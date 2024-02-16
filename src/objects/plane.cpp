#include "plane.hpp"

namespace RT_ISICG
{	
	// M�thode d'intersection de Plane avec prise en compte d'une plage de param�tres t
	bool Plane::intersect( const Ray & p_ray, const float p_tMin, const float p_tMax, HitRecord & p_hitRecord ) const
	{
		float t;
		// V�rifie s'il y a une intersection entre le rayon et le plan
		if ( _geometry.intersect( p_ray, t ) )
		{
			if ( t > p_tMax ) { return false; }  
			if ( t < p_tMin ) { return false; }  
			
			// Met � jour les informations sur l'enregistrement de l'intersection
			p_hitRecord._point	= p_ray.pointAtT( t ); 
			p_hitRecord._normal = _geometry.getNormal();  
			p_hitRecord.faceNormal( p_ray.getDirection() );
			p_hitRecord._distance = t; 
			p_hitRecord._object	  = this; 

			return true;  // Indique qu'il y a eu une intersection dans la plage sp�cifi�e
		}
		return false; 
	}

	// M�thode d'intersection de Plane sans enregistrement des d�tails de l'intersection
	bool Plane::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		float t;

		if ( _geometry.intersect( p_ray, t ) )
		{
			if ( t > p_tMax ) { return false; }
			if ( t < p_tMin ) { return false; }

			return true;
		}
		return false;
	}

} // namespace RT_ISICG
