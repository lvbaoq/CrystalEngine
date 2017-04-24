#pragma once
//Std include
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
// GL includes
#include <app/GL/glew.h>
#include <app/glm/glm.hpp>
#include <app\glm\gtc\matrix_transform.inl>
#include <app\glm\gtc\type_ptr.inl>
//Assimp includes
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
//crystal includes
#include "graphics.h"

#define TEXTURE_DIFFUSE_NAME "texture_diffuse"
#define TEXTURE_SPECULAR_NAME "texture_specular"

namespace crystal {
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	class Mesh {
	public:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;
			setupMesh();
		}

		void bindTextures(Shader& shader,int offset = 0);
		void unBindTextures(Shader& shader, int offset = 0);
		void draw(Shader& shader);
		GLuint getVAO(){ return VAO; }
	private:
		GLuint VAO, VBO, EBO;
		void setupMesh();
	};

	class Model
	{
	private:

		std::vector<Mesh> meshes;
		std::string directory;
		std::vector<Texture> textures_loaded;

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	public:

		//Transform of the model
		Vector3 position;
		Quaternion orientation;
		Vector3 scale;
		bool drawModel;//Whether draw this model or not

		Model(GLchar* path)
		{
			drawModel = true;
			loadModel(path);
			scale = Vector3(1.0f,1.0f,1.0f);
		}

		void draw(Shader& shader);
		std::vector<Mesh>& getMeshList() { return meshes; }
	};

	/*Used to store models for instancing drawing*/
	struct InstanceList
	{
		int modelID;
		int instanceNum;
		std::vector<glm::mat4> modelMatrixList;
		InstanceList(int mid, int num):modelID(mid),instanceNum(num)
		{
			modelMatrixList.reserve(num);
		}
		void draw(Model& model,Shader& shader);
		void setUpVAO(Model& model);
	};
}