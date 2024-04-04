#include "bvh.hpp"
#include "utils/chrono.hpp"

namespace RT_ISICG
{
	void BVH::build( std::vector<TriangleMeshGeometry> * p_triangles )
	{
		std::cout << "Building BVH..." << std::endl;
		if ( p_triangles == nullptr || p_triangles->empty() )
		{
			throw std::exception( "BVH::build() error: no triangle provided" );
		}
		_triangles = p_triangles;

		Chrono chr;
		chr.start();

		_root = new BVHNode();
		_buildRec( _root, 0, _triangles->size(), 0 );

		chr.stop();

		std::cout << "[DONE]: " << chr.elapsedTime() << "s" << std::endl;
	}

	bool BVH::intersect( const Ray & p_ray, const float p_tMin, const float p_tMax, HitRecord & p_hitRecord ) const
	{
		if ( !_root->_aabb.intersect( p_ray, p_tMin, p_tMax ) ) return false;
		return _intersectRec( _root, p_ray, p_tMin, p_tMax, p_hitRecord );
	}

	bool BVH::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		if ( !_root->_aabb.intersect( p_ray, p_tMin, p_tMax ) ) return false;
		return _intersectAnyRec( _root, p_ray, p_tMin, p_tMax );
	}

	void BVH::_buildRec( BVHNode *			p_node,
						 const unsigned int p_firstTriangleId,
						 const unsigned int p_lastTriangleId,
						 const unsigned int p_depth )
	{
		// Calculer l'AABB pour ce nœud en utilisant les triangles dans l'intervalle [p_firstTriangleId,
		// p_lastTriangleId]
		for ( int i = p_firstTriangleId; i < p_lastTriangleId; i++ )
		{
			p_node->_aabb.extend( _triangles->at( i ).getAabb() );
		}

		p_node->_firstTriangleId = p_firstTriangleId;
		p_node->_lastTriangleId	 = p_lastTriangleId;
		p_node->_left			 = nullptr;
		p_node->_right			 = nullptr;

		// Vérifier les critères d'arrêt
		if ( p_depth < _maxDepth && p_lastTriangleId - p_firstTriangleId > _maxTrianglesPerLeaf )
		{
			// Trouver l'AABB du centre
			AABB AABB_centre = AABB();
			for ( int i = p_firstTriangleId; i < p_lastTriangleId; i++ )
				AABB_centre.extend( ( *_triangles )[ i ].getCentroid() );

			// Trouver l'axe de partition en fonction de l'axe ayant la plus grande longueur de l'AABB du nœud
			int	  largestAxis = p_node->_aabb.largestAxis();
			float milieu	  = ( AABB_centre.getMin()[ largestAxis ] + AABB_centre.getMax()[ largestAxis ] ) * 0.5f;

			// Partitionner les triangles en deux groupes en fonction de l'axe de partition
			int idMid = -1;
			for ( int i = p_firstTriangleId; i <= p_lastTriangleId; i++ )
			{
				if ( ( *_triangles )[ i ].getCentroid()[ largestAxis ] >= milieu )
				{
					idMid = i;
					break;
				}
			}
			if ( idMid == -1 ) { idMid = p_lastTriangleId; }

			// Construire les sous-arbres récursivement
			p_node->_left = new BVHNode();
			_buildRec( p_node->_left, p_firstTriangleId, idMid, p_depth + 1 );

			p_node->_right = new BVHNode();
			_buildRec( p_node->_right, idMid, p_lastTriangleId, p_depth + 1 );
		}
	}

	bool BVH::_intersectRec( const BVHNode * p_node,
							 const Ray &	 p_ray,
							 const float	 p_tMin,
							 const float	 p_tMax,
							 HitRecord &	 p_hitRecord ) const
	{
		// Vérifie si le nœud est une feuille
		if ( p_node->isLeaf() )
		{
			// Si le nœud est une feuille, parcourt les triangles dans le nœud pour voir s'il y a une intersection
			float t, tmin = p_tMax + 1;
			int	  id_min = -1;
			Vec3f n; 

			for ( int i = p_node->_firstTriangleId; i < p_node->_lastTriangleId; i++ )
			{
				// Teste l'intersection entre le rayon et le triangle
				if ( _triangles->at( i ).intersect( p_ray, t, n ) )
				{
					// Vérifie si l'intersection est dans le bon intervalle de t et si c'est la plus proche jusqu'à
					// présent
					if ( t <= p_tMax && t >= p_tMin && t < tmin )
					{
						tmin   = t;
						id_min = i;

					}
				}
			}

			// Si une intersection a été trouvée, met à jour p_hitRecord avec les informations de l'intersection
			if ( id_min >= 0 )
			{
				p_hitRecord._point	= p_ray.pointAtT( tmin );
				p_hitRecord._normal = n;
				p_hitRecord.faceNormal( p_ray.getDirection() );
				p_hitRecord._distance = tmin;
				p_hitRecord._object	  = nullptr;

				return true;
			}
			// Sinon, retourne false pour indiquer qu'aucune intersection n'a été trouvée
			return false;
		}
		// Si le nœud n'est pas une feuille
		else
		{
			// Vérifie l'intersection avec les boîtes englobantes des enfants du nœud
			HitRecord ht_left, ht_right;
			bool	  left_intersection = false, right_intersection = false;

			if ( p_node->_left->_aabb.intersect( p_ray, p_tMin, p_tMax ) )
			{
				// Si le rayon intersecte avec la boîte englobante de l'enfant gauche, teste l'intersection avec
				// l'enfant gauche
				left_intersection = _intersectRec( p_node->_left, p_ray, p_tMin, p_tMax, ht_left );
			}

			if ( p_node->_right->_aabb.intersect( p_ray, p_tMin, p_tMax ) )
			{
				// Si le rayon intersecte avec la boîte englobante de l'enfant droit, teste l'intersection avec l'enfant
				// droit
				right_intersection = _intersectRec( p_node->_right, p_ray, p_tMin, p_tMax, ht_right );
			}

			// Traite les cas d'intersection avec les deux enfants, un seul enfant ou aucun enfant
			if ( left_intersection && right_intersection )
			{
				// Si les deux enfants ont une intersection, prend le plus proche des deux et met à jour p_hitRecord
				if ( ht_left._distance < ht_right._distance ) { p_hitRecord = ht_left; }
				else { p_hitRecord = ht_right; }

				return true;
			}
			else if ( left_intersection )
			{
				// Si seulement l'enfant gauche a une intersection, met à jour p_hitRecord avec l'intersection de
				// l'enfant gauche
				p_hitRecord = ht_left;
				return true;
			}
			else if ( right_intersection )
			{
				// Si seulement l'enfant droit a une intersection, met à jour p_hitRecord avec l'intersection de
				// l'enfant droit
				p_hitRecord = ht_right;
				return true;
			}

			// Si aucun des enfants n'a une intersection, retourne false
			return false;
		}
	}
	bool BVH::_intersectAnyRec( const BVHNode * p_node,
								const Ray &		p_ray,
								const float		p_tMin,
								const float		p_tMax ) const
	{
		if ( p_node->isLeaf() )
		{
			Vec3f n;
			float t = -1;
			for ( int i = p_node->_firstTriangleId; i < p_node->_lastTriangleId; i++ )
			{
				if ( _triangles->at( i ).intersect( p_ray, t,n ) && t <= p_tMax && t >= p_tMin ) return true;
			}
			return false;
		}
		else
		{
			bool left_intersection = false;
			if ( p_node->_left->_aabb.intersect( p_ray, p_tMin, p_tMax ) )
				left_intersection = _intersectAnyRec( p_node->_left, p_ray, p_tMin, p_tMax );

			if ( left_intersection ) return true;

			if ( p_node->_right->_aabb.intersect( p_ray, p_tMin, p_tMax ) )
				return _intersectAnyRec( p_node->_right, p_ray, p_tMin, p_tMax );

			return false;
		}
	}
} // namespace RT_ISICG
