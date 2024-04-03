#include "cameras/perspective_camera.hpp"
#include "defines.hpp"
#include "renderer.hpp"
#include "integrators/direct_lighting_integrator.hpp"

namespace RT_ISICG
{
	int main( int argc, char ** argv )
	{
		const int imgWidth	= 600;
		const int imgHeight = 400;

		// Create a texture to render the scene.
		Texture img = Texture( imgWidth, imgHeight );
		std::cout << "Affichage des TPs " << std::endl;
		int input = -1;
		while ( input > 4 || input < 0 )
		{
			std::cout << "------------------------------------" << std::endl;
			std::cout << "1- TP1 - Ray casting" << std::endl;
			std::cout << "2- TP2 - Eclairage et calcul d'ombres portees" << std::endl;
			std::cout << "3- TP3 - Sources lumineuses surfaciques et ombres douces" << std::endl;
			std::cout << "3- TP4 - Triangle, maillage et BVH " << std::endl;
			std::cout << "------------------------------------" << std::endl;
			std::cout << "ENTREZ LE NUMERO DU TP QUE VOUS VOULEZ VOIR" << std::endl;
			std::cin >> input;
			if ( input > 4 || input < 0 ) { std::cout << "ENTREZ LE NUMERO DE TP VALIDE" << std::endl; }
		}
		// Create and init scene.
		Scene scene;
		scene.init(input);

		// Choix de la caméra
		int choixCamera = -1;
		int nbSample	= 32; // à mettre egal à 1 pour activer l'anti-aliasing
		

		// Create a perspective camera.
		Vec3f pos_camera = VEC3F_ZERO;
		Vec3f pos_lookAt = VEC3F_ZERO;

		// Create and setup the renderer.
		Renderer renderer;


		switch ( input )
		{
		case 1: renderer.setIntegrator( IntegratorType::RAY_CAST );
			break;
		case 2: renderer.setIntegrator( IntegratorType::POINT_LIGHT );
			break;
		case 3:
				renderer.setIntegrator( IntegratorType::POINT_LIGHT );
				break;
		case 4:
				renderer.setIntegrator( IntegratorType::POINT_LIGHT ); 
				break;
		default: break;


		}
		switch ( input )
		{
		case 1:
			while ( choixCamera > 4 || choixCamera < 0 )
			{
				std::cout << "------------------------------------" << std::endl;
				std::cout << "1-Pos:(0,0,-2) Look(0,0,79)" << std::endl;
				std::cout << "2-Pos:(1,0,0) Look(1,0,1)" << std::endl;
				std::cout << "3-Pos:(0,1,0) Look(0,1,1)" << std::endl;
				std::cout << "4-Pos:(4,-1,0) Look(-1,-1,2)" << std::endl;
				std::cout << "------------------------------------" << std::endl;
				std::cout << "ENTREZ UN NUMERO DE LA CAMERA" << std::endl;
				std::cin >> choixCamera;
				if ( choixCamera > 4 || choixCamera < 0 )
				{
					std::cout << "ENTREZ LE NUMERO DE CAMERA VALIDE" << std::endl;
				}
			}
			switch ( choixCamera )
			{
			case 1:
				pos_camera = Vec3f( 0.f, 0.f, -2.f );
				pos_lookAt = Vec3f( 0.f, 0.f, 79.f );
				break;
			case 2:
				pos_camera = Vec3f( 1.f, 0.f, 0.f );
				pos_lookAt = Vec3f( 1.f, 0.f, 1.f );
				break;
			case 3:
				pos_camera = Vec3f( 0.f, 1.f, 0.f );
				pos_lookAt = Vec3f( 0.f, 1.f, 1.f );
				break;
			case 4:
				pos_camera = Vec3f( 4.f, -1.f, 0.f );
				pos_lookAt = Vec3f( -1.f, -1.f, 2.f );
				break;
			}
			break;
		case 2:
			pos_camera = Vec3f( 0.f, 0.f, -2.f );
			pos_lookAt = Vec3f( 0.f, 0.f, 79.f );
			break;
		case 3:
			pos_camera = Vec3f( 0.f, 0.f, -2.f );
			pos_lookAt = Vec3f( 0.f, 0.f, 79.f );
			break;
		case 4:
			pos_camera = Vec3f( 0.f, 2.f, -7.f );
			pos_lookAt = Vec3f( 0.f, 0.f, 50.f );
			break;
		default: break;
		}
		
		renderer.setNbPixelSamples( nbSample );
		renderer.setBackgroundColor( GREY );
		
		Vec3f p_up		 = Vec3f( 0.f, 1.f, 0.f );
		float FOV		 = 60.f;
		
		PerspectiveCamera camera( pos_camera, pos_lookAt, p_up, FOV, float( imgWidth ) / imgHeight );


		// Launch rendering.
		std::cout << "Rendering..." << std::endl;
		std::cout << "- Image size: " << imgWidth << "x" << imgHeight << std::endl;

		float renderingTime = renderer.renderImage( scene, &camera, img );

		std::cout << "-> Done in " << renderingTime << "ms" << std::endl;

		// Save rendered image.
		const std::string imgName = "image.jpg";
		img.saveJPG( RESULTS_PATH + imgName );

		return EXIT_SUCCESS;
	}
} // namespace RT_ISICG

int main( int argc, char ** argv )
{
	try
	{
		return RT_ISICG::main( argc, argv );
	}
	catch ( const std::exception & e )
	{
		std::cerr << "Exception caught:" << std::endl << e.what() << std::endl;
	}
}
