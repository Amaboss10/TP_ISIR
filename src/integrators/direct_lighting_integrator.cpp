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


	Vec3f DirectLightingIntegrator::_directLighting( const Scene &		p_scene,
													  const Ray &		p_ray,
													  const HitRecord & hitRecord,
													  const float		p_tMin,
													  const float		p_tMax ) const
	{
		Vec3f Li = VEC3F_ZERO;

		for ( BaseLight * l : p_scene.getLights() )
		{
			LightSample lightSample = l->sample( hitRecord._point );
			Ray			rayShadow	= Ray( hitRecord._point, lightSample._direction );
			rayShadow.offset( hitRecord._normal );

			if ( !p_scene.intersectAny( rayShadow, p_tMin, lightSample._distance ) )
			{
				float angle = glm::max( 0.f, glm::dot( hitRecord._normal, lightSample._direction ) );
				Vec3f color = hitRecord._object->getMaterial()->getFlatColor();
				Li += color * lightSample._radiance * angle;
			}
		}

		return Li;
	}

} // namespace RT_ISICG
