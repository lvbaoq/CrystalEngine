#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GL\glew.h"

#define INFO_LOG_BUFFER_SIZE 512
#define DEFAULT_VETEX_SHADER_PATH "./Shaders/DefaultVertexShader.vs"
#define DEFAULT_FRAGMENT_SHADER_PATH "./Shaders/DefaultFragmentShader.fs"
#define LIGHT_VETEX_SHADER_PATH "./Shaders/LightVertexShader.vs"
#define LIGHT_FRAGMENT_SHADER_PATH "./Shaders/LightFragmentShader.fs"

class Shader
{
public:

	GLuint program;

	Shader(const GLchar* vertexPath = DEFAULT_VETEX_SHADER_PATH, 
		const GLchar* fragmentPath = DEFAULT_FRAGMENT_SHADER_PATH)
	{
		//Read file from file path
		std::string vertexCode, fragmentCode;
		std::ifstream vertexFile, fragmentFile;
		// ensures ifstream objects can throw exceptions:
		vertexFile.exceptions(std::ifstream::badbit);
		fragmentFile.exceptions(std::ifstream::badbit);

		try 
		{
			//Open file
			vertexFile.open(vertexPath);
			fragmentFile.open(fragmentPath);
			std::stringstream vStream, fStream;
			vStream << vertexFile.rdbuf();
			fStream << fragmentFile.rdbuf();
			//Close file handlers
			vertexFile.close();
			fragmentFile.close();
			//Convert to string
			vertexCode = vStream.str();
			fragmentCode = fStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "Fail to read shader files " << e.what() << std::endl;
		}
		//Convert to c string
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		//Compile Shaders		
		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[INFO_LOG_BUFFER_SIZE];
		//vertex
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		//Get log info
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			//Compile error, print log
			glGetShaderInfoLog(vertex, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
			std::cout << "Error compiling vertex shader\n" << infoLog << std::endl;
		}
		//fragment
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		//Get log info
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
			std::cout << "Error compiling fragment shader\n" << infoLog << std::endl;
		}

		//Shader Program
		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
		//Get link info log
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, INFO_LOG_BUFFER_SIZE, NULL, infoLog);
			std::cout << "Error linking program \n" << infoLog << std::endl;
		}
		//Delete shaders
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	GLuint getUniformLocation(const GLchar* const uniformName)
	{
		return glGetUniformLocation(program, uniformName);
	}

	void useShader()
	{
		glUseProgram(this->program);
	}

	void deleteShader()
	{
		glDeleteProgram(this->program);
	}
};