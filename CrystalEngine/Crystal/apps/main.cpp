//Memory leak test
#define _CRTDBG_MAP_ALLOC
#ifdef _DEBUG
//#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include <stdlib.h>
#include <crtdbg.h>

#include <app\app.h>
#include <iostream>
#include <app\defaultVertices.h>

#define MODEL_MATRIX_UNIFORM_NAME "model"
#define VIEW_MATRIX_UNIFORM_NAME "view"
#define PROJECTION_MATRIX_UNIFORM_NAME "projection"
#define COLOR_UNIFORM_NAME "bodyColor"


//The application
extern std::unique_ptr<Application> getApplication();

std::unique_ptr<Application> application;
//Time mark
GLfloat lastFrame = 0.0f;
GLfloat deltaTime = 0.0f;

//Raw call back methods by opengl
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Generates a texture that is suited for attachments to a framebuffer
GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil);

//Render all game objects in current game world.
void renderWorld(Shader &shader,bool depthOnly = false);
void renderInstances(Shader &shader, bool depthOnly = false);

int main()
{
	#pragma region "initialization"
	//Memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//Bind application
	application = getApplication();
	//Init graphics
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	application->initGraphicSettings();

	//Check MSAA settings
	if (application->MSAASampleNum > 0)
	{
		glfwWindowHint(GLFW_SAMPLES, application->MSAASampleNum);
	}
	
	GLFWwindow* window = glfwCreateWindow(application->screenWidth, application->screenHeight, 
		application->getTitle(), nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	application->glWindow = window;
	
	//Setup glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
	}
	glfwMakeContextCurrent(window);

	//Call application's initialization method after setup
	application->initGraphics(window);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	//Add callback methods
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
#pragma endregion

	//Init shaders with lights enabled
	Shader shader{LIGHT_VETEX_SHADER_PATH,LIGHT_FRAGMENT_SHADER_PATH};
	Shader skyboxShader{ DEFAULT_SKYBOX_VERTEX_SHADER_PATH,DEFAULT_SKYBOX_FRAGMENT_SHADER_PATH };
	Shader postShader{ POST_PROCESSING_VSHADER_PATH,POST_PROCESSING_FRAG_PATH_NORMAL };
	Shader depthShader{ DEPTH_VERTEX_PATH ,EMPTY_FRAG_PATH };
	Shader treeShader{VERTEX_SHADER_INSTANCE,LIGHT_FRAGMENT_SHADER_PATH };
	Shader instanceDepthShader{ DEPTH_SHADER_INSTANCE ,EMPTY_FRAG_PATH };

	#pragma region "setVAOs"
	//Put vertices of primitive shapes into vetex buffer
	//Boxes
	GLuint boxVBO, boxVAO;
	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);
	glGenBuffers(1, &boxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Box::boxVAO = boxVAO;
	
	//Square
	GLuint squareVBO, squareVAO;
	glGenVertexArrays(1, &squareVAO);
	glBindVertexArray(squareVAO);
	glGenBuffers(1, &squareVBO);
	glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Explosion::particleVAO = squareVAO;
	Plane::planeVAO = squareVAO;

	//Skybox
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Setup screen VAO. Screen vao is used to show post processing results
	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	//Setup Framebuffers
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Create a color attachment texture
	GLuint textureColorbuffer = generateAttachmentTexture(false, false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// Create a renderbuffer object for depth and stencil attachment
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	// Use a single renderbuffer object for both a depth and stencil buffer.
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, application->screenWidth, application->screenHeight); 
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
	//Check if complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Depth map
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	//create depth texture
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, application->depthMapWidth, application->depthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	//Only depth value is needed
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	//Check if complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER::Depth Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Set texture units
	shader.useShader();
	glUniform1i(glGetUniformLocation(shader.program, "material.texture_diffuse1"), 0);
	glUniform1i(glGetUniformLocation(shader.program, "material.texture_specular1"), 1);
	glUniform1i(glGetUniformLocation(shader.program, "material.texture_emission"), 2);
	glUniform1i(glGetUniformLocation(shader.program, "shadowMap"), 3);
	glUseProgram(0);
	treeShader.useShader();
	glUniform1i(glGetUniformLocation(treeShader.program, "material.texture_diffuse1"), 4);
	glUniform1i(glGetUniformLocation(treeShader.program, "material.texture_specular1"), 5);
	glUniform1i(glGetUniformLocation(treeShader.program, "material.texture_emission"), 6);
	glUniform1i(glGetUniformLocation(treeShader.program, "shadowMap"), 7);
	glUseProgram(0);

#pragma endregion

	//Set up application
	application->start();

	// Set Vertex Buffer Object for instancing
	for (crystal::InstanceList instance : application->instanceList)
	{
		instance.setUpVAO(application->getModel(instance.modelID));
	}

	//Load skybox
	application->skybox.loadCubeMap();

	// Draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Main Game Loop
	while (!glfwWindowShouldClose(window))
	{
	#pragma region "Game Loop"
		//Mark Time
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;
		if (deltaTime <= 0.0f) continue;
		else if (deltaTime > 0.05f) deltaTime = 0.05f;
		//Trigger Events
		glfwPollEvents();
		//Pause Game
		if (application->pause) continue;

		//Update
		application->world->startFrame();
		application->update(deltaTime);
		//Run Physics
		if (application->runPhysics)
		{
			application->world->runPhysics(deltaTime);
		}

		//Render to depth texture from dirLight's perspective
		glCullFace(GL_FRONT);//Peter panning
		
		depthShader.useShader();
		glUniformMatrix4fv(glGetUniformLocation(depthShader.program, "lightSpaceMatrix"),
			1, GL_FALSE, glm::value_ptr(application->dirLight->getLightSpaceMatrix()));
		glViewport(0, 0, application->depthMapWidth, application->depthMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		renderWorld(depthShader,true);
		
		//Instance
		if (!application->instanceList.empty())
		{
			instanceDepthShader.useShader();
			glUniformMatrix4fv(glGetUniformLocation(instanceDepthShader.program, "lightSpaceMatrix"),
				1, GL_FALSE, glm::value_ptr(application->dirLight->getLightSpaceMatrix()));
			glViewport(0, 0, application->depthMapWidth, application->depthMapHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			renderInstances(instanceDepthShader, true);
		}		

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);

		//Reset viewport
		glViewport(0, 0, application->screenWidth, application->screenHeight);

		//Clear Screen
		glm::vec4 cColor = application->clearColor;
		glClearColor(cColor.x, cColor.y, cColor.z, cColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw
		if (application->postEffect)
		{
			//Some postEffect is enabled
			if (!application->postEffect->isShaderInitialized())
			{
				application->postEffect->initShader();
			}
		}
		
		//Use post process here to enable gamma correction
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		//Common objects in game world
		shader.useShader();		
		
		//Set direction light
		if (application->dirLight)
		{
			application->dirLight->setLightUniform(shader.program);
		}

		GLuint viewLoc = shader.getUniformLocation(VIEW_MATRIX_UNIFORM_NAME);
		GLuint projectionLoc = shader.getUniformLocation(PROJECTION_MATRIX_UNIFORM_NAME);
		//view matrix
		glm::mat4 view;
		view = application->camera->getViewMarixAfterMoving();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		//projection matrix
		glm::mat4 projection = application->camera->getProjectionMatrix();	
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		renderWorld(shader);
		
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, depthTexture);

		//Update particles
		application->pworld->runPhysics(deltaTime);
		
		//Draw Particles
		Explosion::program = shader.program;
		Explosion::modelLoc = shader.getUniformLocation(MODEL_MATRIX_UNIFORM_NAME);
		glDisable(GL_CULL_FACE);
		for (crystal::PEffectPtr pe : application->pworld->particleEffects)
		{
			pe->drawEffect(deltaTime);
		}
		glEnable(GL_CULL_FACE);
		//Draw Instances
		if (!application->instanceList.empty())
		{
			treeShader.useShader();
			//Set direction light
			if (application->dirLight)
			{
				application->dirLight->setLightUniform(treeShader.program);
			}
			//Set view and projection matrix
			glUniformMatrix4fv(glGetUniformLocation(treeShader.program, VIEW_MATRIX_UNIFORM_NAME), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(treeShader.program, PROJECTION_MATRIX_UNIFORM_NAME), 1, GL_FALSE, glm::value_ptr(projection));
			renderInstances(treeShader);
		}

		//Draw Skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.useShader();
		//Set light color for skybox
		glUniform3f(glGetUniformLocation(skyboxShader.program, "dirLightDiffuse")
			,application->dirLight->diffuse.x, application->dirLight->diffuse.y, application->dirLight->diffuse.z);

		glm::mat4 skyBoxview = glm::mat4(glm::mat3(application->camera->getViewMarixAfterMoving()));	// Remove any translation component of the view matrix
		glm::mat4 skyBoxProjection = application->camera->getProjectionMatrix();
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.program, "view"), 1, GL_FALSE, glm::value_ptr(skyBoxview));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(skyBoxProjection));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxShader.program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, application->skybox.getCubeTexture());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		if (application->postEffect)
		{
			//Some postEffect is enabled
			application->postEffect->useShader();
		}
		else 
		{
			postShader.useShader();		
		}
		//Use opengl built-in gamma correction methods
		if (application->useGammaCorrection)
		{
			glEnable(GL_FRAMEBUFFER_SRGB);
		}
		else
		{
			glDisable(GL_FRAMEBUFFER_SRGB);
		}

		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		
		//Finish draw all objects
		glfwSwapBuffers(window);

	#pragma endregion
	}

	//Properly release gl resources
	glDeleteBuffers(1, &boxVBO);
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteBuffers(1, &squareVBO);
	glDeleteVertexArrays(1, &squareVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteFramebuffers(1, &framebuffer);
	shader.deleteShader();
	skyboxShader.deleteShader();
	treeShader.deleteShader();
	glfwTerminate();

	return 0;
}

void renderWorld(Shader &shader,bool depthOnly)
{
	GLuint modelLoc = shader.getUniformLocation(MODEL_MATRIX_UNIFORM_NAME);

	GLuint colorLoc = shader.getUniformLocation(COLOR_UNIFORM_NAME);

	glEnable(GL_CULL_FACE);
	
	application->display();//Common Stuff
	//Draw each rigidbody
	crystal::RigidBodyList bodyList = application->world->getRigidBodyList();

	for (auto body : bodyList)
	{
		if (body->isActive)
		{
			if (body->isPrimitive())
			{
				//Only draw primitive shapes for now
				Primitive* shape = (Primitive*)(body.get());
				glBindVertexArray(shape->getVAO());
				glm::mat4 model = shape->getModelMatrix();
				//glm::mat4 model;
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				//Material
				if (!depthOnly)
				{
					setMaterialUniform(shape->getMaterial(), shader.program);
				}
				//Draw Call
				glDrawArrays(GL_TRIANGLES, 0, shape->getVertexNumber());
			}
		}
	}

	glBindVertexArray(0);

	//Draw model
	for (int i = 0; i < application->modelList.size(); i++)
	{
		application->modelList[i].draw(shader);
	}
}

void renderInstances(Shader &shader, bool depthOnly)
{
	shader.useShader();
	glEnable(GL_CULL_FACE);
	//Draw Instances
	for (crystal::InstanceList instances : application->instanceList)
	{
		instances.draw(application->getModel(instances.modelID), shader);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	application->key(key, action);
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	application->mouseMove(xpos,ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	application->scroll(xoffset,yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	application->mouseButton(button, action);
}

// Generates a texture that is suited for attachments to a framebuffer
GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil)
{
	// What enum to use?
	GLenum attachment_type;
	if (!depth && !stencil)
		attachment_type = GL_RGB;
	else if (depth && !stencil)
		attachment_type = GL_DEPTH_COMPONENT;
	else if (!depth && stencil)
		attachment_type = GL_STENCIL_INDEX;

	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (!depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, application->screenWidth, application->screenHeight, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	else // Using both a stencil and depth test, needs special format arguments
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, application->screenWidth, application->screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}
