#pragma once
#include <crystal\core.h>
//Glew
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // !GLEW_STATIC

#define DEFAULT_TEXTURE_WHITE_PATH "./DefaultAssets/Textures/DefaultTexture.png"
#define DEFAULT_TEXTURE_BLACK_PATH "./DefaultAssets/Textures/DefaultTextureBlack.png"
#define SKYBOX_FACES_NUMBER 6
#define DEFAULT_SKYBOX_FACE_RIGHT "./DefaultAssets/Skybox/right.jpg"
#define DEFAULT_SKYBOX_FACE_LEFT "./DefaultAssets/Skybox/left.jpg"
#define DEFAULT_SKYBOX_FACE_TOP "./DefaultAssets/Skybox/top.jpg"
#define DEFAULT_SKYBOX_FACE_BOTTOM "./DefaultAssets/Skybox/bottom.jpg"
#define DEFAULT_SKYBOX_FACE_BACK "./DefaultAssets/Skybox/back.jpg"
#define DEFAULT_SKYBOX_FACE_FRONT "./DefaultAssets/Skybox/front.jpg"


#include <app\GL\glew.h>
#include "SOIL\SOIL.h"
namespace crystal
{
	class Texture
	{

	private:
		int width;
		int height;
		GLuint texture;

		GLuint wrap_s_model;
		GLuint wrap_t_model;
		GLuint min_filter_model;
		GLuint mag_filter_model;

		bool created;

	public:
		/**
		* Create a texture using given texture location
		* The image is then binded to the texture object 'texture'
		*/
		void createTexture(const char* const location);

		Texture() :texture(0), created(false), wrap_s_model(GL_REPEAT), wrap_t_model(GL_REPEAT),
			min_filter_model(GL_LINEAR_MIPMAP_LINEAR), mag_filter_model(GL_NEAREST_MIPMAP_NEAREST)
		{}

		GLuint getTexture()
		{
			return texture;
		}

		bool isCreated()
		{
			return created;
		}

	};

	/* Representing different materials  */
	class Material
	{
	public:
		Vector3 ambient;//Ambient color
		Vector3 diffuse;//Diffuse color
		Vector3 specular;//Specular color

		Texture diffuseMap;//Diffuse texture
		Texture specularMap;//Specular texture
		Texture emissionMap;//Emission Map

		float shininess;//Impacts the scattering/radius of the specular highlight

		Material() { *this = defaultMaterial; }

		Material(Vector3 ambient, Vector3 diffuse, Vector3 specular, float shininess) :
			ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}

		void setDiffuseMap(const char* const path);
		void setSpecularMap(const char* const path);
		void setEmissionMap(const char* const path);

		void initTextures();

		//Some pre-set materials
		const static Material emerald;
		const static Material jade;
		const static Material obsidian;
		const static Material pearl;
		const static Material ruby;
		const static Material defaultMaterial;
		static Material pureColorMaterial(Vector3 color);
	};

	class SkyBox
	{
	private:
		GLuint textureCube;
		int width;
		int height;
		char* faces[SKYBOX_FACES_NUMBER];
	public:
		void loadCubeMap();
		SkyBox();
		GLuint getCubeTexture()
		{
			return textureCube;
		}
	};

}