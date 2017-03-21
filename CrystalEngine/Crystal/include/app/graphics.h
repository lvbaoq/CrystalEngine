#pragma once
#include <crystal\core.h>
namespace crystal
{
	/* Representing different materials  */
	class Material
	{
	public:
		Vector3 ambient;//Ambient color
		Vector3 diffuse;//Diffuse color
		Vector3 specular;//Specular color
		float shininess;//Impacts the scattering/radius of the specular highlight

		Material() { *this = defaultMaterial; }

		Material(Vector3 ambient, Vector3 diffuse, Vector3 specular, float shininess) :
			ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}

		//Some pre-set materials
		const static Material emerald;
		const static Material jade;
		const static Material obsidian;
		const static Material pearl;
		const static Material ruby;
		const static Material defaultMaterial;
		static Material pureColorMaterial(Vector3 color);
	};
}