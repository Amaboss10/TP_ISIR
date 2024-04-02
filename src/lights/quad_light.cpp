#include "quad_light.hpp"
#include "utils/random.hpp"

namespace RT_ISICG
{
	LightSample QuadLight::sample( const Vec3f & p_point ) const
	{

		Vec3f posInQuad = _position + randomFloat() * _u + randomFloat() * _v;
		float distance = glm::distance( p_point, posInQuad );
		Vec3f direction = glm::normalize( p_point - posInQuad );
		float pdf		  = ( 1 / _area ) * ( ( distance * distance ) / glm::dot( _normal, -direction ) );
		Vec3f radiance  = ( _color * _power ) / pdf;

		return LightSample( direction, distance, radiance, pdf );
		
	}
} // namespace RT_ISICG