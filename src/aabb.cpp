#include "aabb.hpp"

namespace RT_ISICG
{
	bool AABB::intersect( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		float t0 = p_tMin;
		float t1 = p_tMax;

		// Parcours des trois dimensions de la boîte englobante.
		for ( int i = 0; i < 3; ++i )
		{
			// Calcul de l'inverse de la direction du rayon pour cette dimension.
			float invDir = 1.0f / p_ray.getDirection()[ i ];

			// Calcul des distances d'intersection avec les plans de la boîte englobante pour cette dimension.
			float tNear = ( _min[ i ] - p_ray.getOrigin()[ i ] ) * invDir;
			float tFar	= ( _max[ i ] - p_ray.getOrigin()[ i ] ) * invDir;

			// Vérification de l'ordre des distances d'intersection.
			if ( tNear > tFar ) std::swap( tNear, tFar );

			// Mise à jour des distances d'intersection les plus proches et les plus éloignées.
			t0 = ( tNear > t0 ) ? tNear : t0;
		 	t1 = ( tFar < t1 ) ? tFar : t1;

			if ( t0 > t1 ) return false;
		}

		return true;
	}
} // namespace RT_ISICG
