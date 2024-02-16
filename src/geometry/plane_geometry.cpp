#include "plane_geometry.hpp"

namespace RT_ISICG
{
	bool PlaneGeometry::intersect( const Ray & p_ray, float & p_t ) const
	{
		/* Calcule l'�quation de l'intersection entre le plan et le rayon
		 * On calcule:
		 *	le vecteur de l'origine du rayon au point du plan
		 *	le produit scalaire entre le vecteur po et le vecteur normal du plan
		 *	le produit scalaire entre la direction du rayon et le vecteur normal du plan
		 *	le param�tre t pour lequel le rayon intersecte le plan
		 * 
		 * On v�rifie si l'intersection se produit en avant de l'origine du rayon if (t > 0)
		 */

		Vec3f po		   = ( this->_point - p_ray.getOrigin() ); 
		float numerateur   = glm::dot( po, this->_normal ); 
		float denominateur = glm::dot( p_ray.getDirection(), this->_normal );
		float t			   = numerateur / denominateur;

		if ( t > 0 )
		{	
		    // Met � jour le param�tre t avec la valeur calcul�e
			p_t = t;
			
			// Retourne true pour indiquer qu'il y a eu une intersection
			return true;
		}
		// Retourne false si l'intersection se produit derri�re l'origine du rayon
		return false;
	}

} // namespace RT_ISICG