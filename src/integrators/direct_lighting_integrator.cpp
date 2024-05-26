#include "direct_lighting_integrator.hpp"

namespace RT_ISICG
{
	Vec3f DirectLightingIntegrator::Li( const Scene & p_scene,
										const Ray &	  p_ray,
										const float	  p_tMin,
										const float	  p_tMax ) const
	{
		HitRecord hitRecord;
		if ( p_scene.intersect( p_ray, p_tMin, p_tMax, hitRecord ) )
		{
			Vec3f Li = _directLighting( p_scene, p_ray, hitRecord, p_tMin, p_tMax );
			return Li;
		}
		else { return _backgroundColor; }
	}

	Vec3f DirectLightingIntegrator::_directLighting( const Scene &	   p_scene,
													 const Ray &	   p_ray,
													 const HitRecord & hitRecord,
													 const float	   p_tMin,
													 const float	   p_tMax ) const
	{
		Vec3f Li = VEC3F_ZERO;

		for ( BaseLight * l : p_scene.getLights() )
		{
			int _nbLightSamples
				= ( l->getIsSurface() ) ? 32 : 1; // Nombre d'échantillons en fonction du type de source lumineuse

			Vec3f tempLi = VEC3F_ZERO;
			for ( int i = 0; i < _nbLightSamples; i++ )
			{
				LightSample lightSample = l->sample( hitRecord._point );
				Ray			shadowRay	= ( l->getIsSurface() ) ? Ray( hitRecord._point, -lightSample._direction )
																: Ray( hitRecord._point, lightSample._direction );
				shadowRay.offset( hitRecord._normal );

				if ( !p_scene.intersectAny( shadowRay, p_tMin, lightSample._distance ) )
				{
					float cosTheta = ( l->getIsSurface() ) ? glm::dot( hitRecord._normal, -lightSample._direction )
														   : glm::dot( hitRecord._normal, lightSample._direction );
					tempLi += hitRecord._object->getMaterial()->shade(p_ray, hitRecord, lightSample) * lightSample._radiance
							  * glm::max( 0.f, cosTheta );
				}
			}

			Li += tempLi / (float)( _nbLightSamples );
		}

		return Li;
	}
} // namespace RT_ISICG
