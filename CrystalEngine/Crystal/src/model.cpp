#include <app\model.h>

using namespace std;
using namespace crystal;

void crystal::Mesh::bindTextures(Shader & shader,int offset)
{
	//Bind texture
	int diffuseN = 1;
	int specularN = 1;
	for (GLuint i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + offset + i);
		//Retrieve texture number
		stringstream ss;
		string number;
		string name = textures[i].type;
		if (name == TEXTURE_DIFFUSE_NAME)
			ss << diffuseN++;
		else if (name == TEXTURE_SPECULAR_NAME)
			ss << specularN++;
		number = ss.str();
		glUniform1i(glGetUniformLocation(shader.program, ("material." + name + number).c_str()), i+offset);
		glBindTexture(GL_TEXTURE_2D, textures[i].getTexture());
	}
	glUniform1f(glGetUniformLocation(shader.program, "material.shininess"), 16.0f);
	//No color effect
	GLint matAmbientLoc = glGetUniformLocation(shader.program, "material.ambientColor");
	GLint matDiffuseLoc = glGetUniformLocation(shader.program, "material.diffuseColor");
	GLint matSpecularLoc = glGetUniformLocation(shader.program, "material.specularColor");
	GLint matShineLoc = glGetUniformLocation(shader.program, "material.shininess");

	glUniform3f(matAmbientLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(matDiffuseLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(matSpecularLoc, 1.0f, 1.0f, 1.0f);
}

void crystal::Mesh::unBindTextures(Shader & shader,int offset)
{
	//Set back to default
	for (GLuint i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i+offset);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::draw(Shader& shader)
{
	bindTextures(shader);

	//draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	unBindTextures(shader);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	//set attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	//check error
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	//Recursive process all children nodes
	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	
	//Get vertices
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// Normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		
		//Texture coordinates
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			//Only first set of coordinates is needed
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	//Get faces (indices)
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//Get materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		//Diffuse maps
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE_NAME);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//Specular maps
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TEXTURE_SPECULAR_NAME);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	return std::move(Mesh(vertices,indices,textures));
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	//Store result
	vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		//get texture location
		aiString str;
		mat->GetTexture(type, i, &str);
		//check if the texture was already loaded
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); j++)
		{
			if (strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			string texturePath = directory + '/' + string(str.C_Str());
			texture.createTexture(texturePath.c_str(),true);//Model textures are usually in sRGB form
			texture.type = typeName;
			texture.path = string(str.C_Str());
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return std::move(textures);
}

void crystal::Model::draw(Shader & shader)
{
	if (!drawModel) return;
	//Transform
	//Transition
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
	//Rotation
	if (!orientation.noRoatation())
	{
		float rAngel = 2 * real_acos(orientation.r);
		float aX = orientation.i / real_sqrt(1 - orientation.r*orientation.r);
		float aY = orientation.j / real_sqrt(1 - orientation.r*orientation.r);
		float aZ = orientation.k / real_sqrt(1 - orientation.r*orientation.r);
		glm::vec3 glRotation(aX, aY, aZ);
		model = glm::rotate(model, rAngel, glRotation);
	}
	//Scale
	model = glm::scale(model, glm::vec3(scale.x,scale.y, scale.z));
	//Set uniform
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].draw(shader);
	}
}

void crystal::InstanceList::draw(Model& model,Shader & shader)
{
	for (GLuint i = 0; i < model.getMeshList().size(); i++)
	{
		model.getMeshList()[i].bindTextures(shader,4);
		glBindVertexArray(model.getMeshList()[i].getVAO());
		glDrawElementsInstanced(
			GL_TRIANGLES, model.getMeshList()[i].indices.size(), GL_UNSIGNED_INT, 0, instanceNum
		);
		glBindVertexArray(0);
		model.getMeshList()[i].unBindTextures(shader,4);
	}
	/*
	for (glm::mat4 matrix : modelMatrixList)
	{
		//Set uniform
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

		for (int i = 0; i < model.getMeshList().size(); i++)
		{
			model.getMeshList()[i].draw(shader);
		}
	}
	*/
}

void crystal::InstanceList::setUpVAO(Model& model)
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, instanceNum * sizeof(glm::mat4), &modelMatrixList[0], GL_STATIC_DRAW);

	for (GLuint i = 0; i < model.getMeshList().size(); i++)
	{
		GLuint VAO = model.getMeshList()[i].getVAO();
		glBindVertexArray(VAO);
		// Vertex Attributes
		GLsizei vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (GLvoid*)(3 * vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}
