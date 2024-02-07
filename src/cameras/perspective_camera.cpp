#include "perspective_camera.hpp"
#include <glm/gtx/string_cast.hpp>

namespace RT_ISICG
{
	PerspectiveCamera::PerspectiveCamera( const float p_aspectRatio ) : _aspectRatio( p_aspectRatio )
	{
		_updateViewport();
	}

	PerspectiveCamera::PerspectiveCamera( const Vec3f & p_position,
										  const Vec3f & p_lookAt,
										  const Vec3f & p_up,
										  const float	p_fovy,
										  const float	p_aspectRatio )
		: BaseCamera( p_position ), _fovy( p_fovy ), _aspectRatio( p_aspectRatio )
	{
		_w = -glm::normalize( p_lookAt - p_position );
		_u = glm::normalize( glm::cross( p_up, _w ) );
		_v = glm::normalize( glm::cross( _w, _u ) );
	
		_updateViewport();
	}

	void PerspectiveCamera::_updateViewport()
	{
		/// TODO ! _viewportTopLeftCorner ?	_viewportU ? _viewportV ?
		float viewPortHeight = (glm::tan(glm::radians(_fovy) ) * _focalDistance);
		float viewPortWidth	 = viewPortHeight * this->_aspectRatio;

		_viewportU = viewPortWidth * _u;
		_viewportV = viewPortHeight * _v;

		_viewportTopLeftCorner =getPosition() - _focalDistance * _w + 0.5f * _viewportV - 0.5f * _viewportU;

	}

} // namespace RT_ISICG
