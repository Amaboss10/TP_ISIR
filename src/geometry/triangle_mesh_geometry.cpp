#include "triangle_mesh_geometry.hpp"
#include "objects/triangle_mesh.hpp"

namespace RT_ISICG
{
	TriangleMeshGeometry::TriangleMeshGeometry( const unsigned int p_v0,
												const unsigned int p_v1,
												const unsigned int p_v2,
												MeshTriangle *	   p_refMesh )
		: _v0( p_v0 ), _v1( p_v1 ), _v2( p_v2 ), _refMesh( p_refMesh )
	{
		_faceNormal = glm::normalize( glm::cross( _refMesh->_vertices[ p_v1 ] - _refMesh->_vertices[ p_v0 ],
												  _refMesh->_vertices[ p_v2 ] - _refMesh->_vertices[ p_v0 ] ) );
	}

	bool TriangleMeshGeometry::intersect( const Ray & p_ray, float & p_t, Vec3f & p_n ) const
	{	
		// Petit epsilon pour gérer les erreurs d'arrondi
		const float EPSILON = 0.0000000001f;

		const Vec3f & o	 = p_ray.getOrigin();
		const Vec3f & d	 = p_ray.getDirection();
		const Vec3f & v0 = _refMesh->_vertices[ _v0 ];
		const Vec3f & v1 = _refMesh->_vertices[ _v1 ];
		const Vec3f & v2 = _refMesh->_vertices[ _v2 ];


		// Calcul des arêtes du triangle
		const Vec3f edge1 = v1 - v0;
		const Vec3f edge2 = v2 - v0;

		// Calcul du produit vectoriel entre la direction du rayon et la deuxième arête
		const Vec3f pvec = glm::cross( d, edge2 );
		// Calcul du déterminant
		const float det = glm::dot( edge1, pvec );

		// Vérifier si le déterminant est proche de zéro (triangle coplanaire)
		if ( det > -EPSILON && det < EPSILON ) return false;

		// Calcul de l'inverse du déterminant
		const float inv_det = 1.f / det;
		// Calcul du vecteur de distance entre l'origine du rayon et le premier sommet du triangle
		const Vec3f tvec = o - v0;
		// Calcul du produit vectoriel entre le vecteur de distance et la première arête
		const Vec3f qvec = glm::cross( tvec, edge1 );

		// Calcul des coordonnées barycentriques u et v
		float u = glm::dot( tvec, pvec ) * inv_det;
		// Vérifier si u est dans l'intervalle [0, 1]
		if ( u < 0.f || u > 1.f ) return false;

		float v = glm::dot( d, qvec ) * inv_det;
		// Vérifier si v est dans l'intervalle [0, 1] et si u + v est inférieur à 1
		if ( v < 0.f || u + v > 1.f ) return false;

		// Calcul de la distance le long du rayon jusqu'au point d'intersection
		float t = inv_det * glm::dot( edge2, qvec );
		// Vérifier si la distance est positive et supérieure à EPSILON
		if ( t > EPSILON )
		{
			// Interpolation des normales des sommets du triangle
			Vec3f interpolatedNormal = ( 1.0f - u - v ) * _refMesh->_normals[ _v0 ] + u * _refMesh->_normals[ _v1 ]
									   + v * _refMesh->_normals[ _v2 ];
			// Normalisation de la normale interpolée
			interpolatedNormal = glm::normalize( interpolatedNormal );
			// Stockage de la normale interpolée
			p_n = interpolatedNormal;
			// Stockage de la distance d'intersection
			p_t = t;
			return true;
		}
		else
		{
			// Aucune intersection valide, retourner false
			return false;
		}
	}

} // namespace RT_ISICG
