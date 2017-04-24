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

//Shader paths
#define POST_PROCESSING_VSHADER_PATH "./DefaultAssets/Shaders/PostProcessingVertexShader.vs"
#define POST_PROCESSING_FRAG_PATH_INVERSION "./DefaultAssets/Shaders/InversionEffect.fs"
#define POST_PROCESSING_FRAG_PATH_GRAYSCALE "./DefaultAssets/Shaders/GrayScaleEffect.fs"
#define POST_PROCESSING_FRAG_PATH_KERNEL "./DefaultAssets/Shaders/KernelEffect.fs"
#define POST_PROCESSING_FRAG_PATH_NORMAL "./DefaultAssets/Shaders/PostProcessingNormal.fs"
#define DEPTH_VERTEX_PATH "./DefaultAssets/Shaders/defaultDepthShader.vs"
#define EMPTY_FRAG_PATH "./DefaultAssets/Shaders/emptyFragmentShader.fs"
#define VERTEX_SHADER_INSTANCE "./DefaultAssets/Shaders/TreeVertexShader.vs"
#define DEPTH_SHADER_INSTANCE "./DefaultAssets/Shaders/InstanceDepthShader.vs"

#define KERNEL_VALUE_NUM 9
#define DEFAULT_KERNEL_OFFSET (1.0f/300.0f)
#define KERNEL_OFFSET_UNIFORM "offset"
#define KERNEL_UNIFORM "kernel"

#include <app\GL\glew.h>
#include <app\SOIL\SOIL.h>
#include <app\glm\glm.hpp>
#include <app\shader.h>
#include <string>

namespace crystal
{
	/*Representing a texture*/
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
		std::string path;
		std::string type;
		/**
		* Create a texture using given texture location
		* The image is then binded to the texture object 'texture'
		*/
		void createTexture(const char* const location, bool isSRGB = false);

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

	/*Representing a skybox*/
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

	/*Representing some post processing effects*/
	class PostProcessing
	{
	private:
		Shader shader;
		GLfloat kernel[9];//Used for kernel effects
		GLfloat kernelOffset;
		bool isKernel;
		bool shaderinitialized;
		const char* fShaderPath;
	public:
		//Some pre-defined effects
		static PostProcessing Inversion;
		static PostProcessing Grayscale;
		static PostProcessing Sharpen;
		static PostProcessing Blur;
		static PostProcessing EdgeDetection;
		static PostProcessing KernelEffect;//For customized effects

		PostProcessing(const char* fragmentShaderPath);
		PostProcessing(GLfloat* kernel);
		~PostProcessing()
		{ 
			//Delete shader
			shader.deleteShader(); 
		}

		void initShader();

		void useShader();

		Shader& getShader()
		{
			return shader;
		}

		GLfloat getKernelOffset()
		{
			return kernelOffset;
		}

		bool isShaderInitialized()
		{
			return shaderinitialized;
		}

	};
}