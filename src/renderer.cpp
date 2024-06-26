#include "renderer.hpp"
#include "integrators/ray_cast_integrator.hpp"
#include "utils/console_progress_bar.hpp"
#include "utils/random.hpp"
#include "integrators/direct_lighting_integrator.hpp"

namespace RT_ISICG
{
	Renderer::Renderer() { _integrator = new RayCastIntegrator(); }

	void Renderer::setIntegrator( const IntegratorType p_integratorType )
	{
		if ( _integrator != nullptr ) { delete _integrator; }

		switch ( p_integratorType )
		{
		case IntegratorType::RAY_CAST:
		{
			_integrator = new RayCastIntegrator();
			break;
		}
		case IntegratorType::POINT_LIGHT:
		{
			_integrator = new DirectLightingIntegrator();
			break;
		}
		default: break;

		}
	}

	void Renderer::setBackgroundColor( const Vec3f & p_color )
	{
		if ( _integrator == nullptr ) { std::cout << "[Renderer::setBackgroundColor] Integrator is null" << std::endl; }
		else
		{
			_integrator->setBackgroundColor( p_color );
		}
	}

	float Renderer::renderImage( const Scene & p_scene, const BaseCamera * p_camera, Texture & p_texture )
	{
		const int width	 = p_texture.getWidth();
		const int height = p_texture.getHeight();

		Chrono			   chrono;
		ConsoleProgressBar progressBar;

		progressBar.start( height, 50 );
		chrono.start();
		float pMin = 0, pMax = 700;
		#pragma omp parallel for
		for ( int j = 0; j < height; j++ )
		{
			for ( int i = 0; i < width; i++ )
			{
				
				// G�n�rer un rayon passant par le centre de chaque pixel
				//Ray ray = p_camera->generateRay( float( i ) / float ( width - 1 ), float( j ) / float ( height - 1 ));

				// Appel de la methode LI 
				//Vec3f color =   _integrator->Li( p_scene, ray, pMin, pMax );

				//Vec3f color = ( ray.getDirection() + 1.f ) * 0.5f;

				// Appliquer la couleur au pixel de la texture
				//p_texture.setPixel( i, j, color );

				Vec3f color = Vec3f( 0.f );

				for ( int sample = 0; sample < _nbPixelSamples; sample++ )
				{

					const Ray myRay = p_camera->generateRay( (float)( i  + randomFloat()) / (float)( width - 1),
															 (float)( j  + randomFloat() )/ (float)( height -1 ) );

					color += _integrator->Li( p_scene, myRay, pMin, pMax );
				}
				Vec3f colorfinal = color / (float)_nbPixelSamples;
				// std::cout << color.x << " " << color.y << " " << color.z << std::endl;
				p_texture.setPixel( i, j, glm::clamp( colorfinal, 0.f, 1.f ) );

			}
			progressBar.next();
		}

		chrono.stop();
		progressBar.stop();

		return chrono.elapsedTime();
	}
} // namespace RT_ISICG
