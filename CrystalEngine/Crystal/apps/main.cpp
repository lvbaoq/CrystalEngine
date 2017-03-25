//Memory leak test
#define _CRTDBG_MAP_ALLOC
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#include <stdlib.h>
#include <crtdbg.h>

#include <app\app.h>
#include <iostream>
#include <app\shader.h>
#include <app\glm\gtc\type_ptr.inl>
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

	//Set texture units
	shader.useShader();
	glUniform1i(glGetUniformLocation(shader.program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shader.program, "material.specular"), 1);
	glUniform1i(glGetUniformLocation(shader.program, "material.emission"), 2);
	glUseProgram(0);
#pragma endregion

	//Set up application
	application->start();

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
			application->world->startFrame();
			application->world->runPhysics(deltaTime);
		}

		//Clear Screen
		glm::vec4 cColor = application->clearColor;
		glClearColor(cColor.x, cColor.y, cColor.z, cColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw
		if (application->postEffect)
		{
			if (!application->postEffect->isShaderInitialized())
			{
				application->postEffect->initShader();
			}
			//Some postEffect is enabled
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
		}
		//Common objects
		shader.useShader();		
		//Set direction light
		if (application->dirLight)
		{
			application->dirLight->setLightUniform(shader.program);
		}
		//Perform transform
		GLuint modelLoc = shader.getUniformLocation(MODEL_MATRIX_UNIFORM_NAME);
		GLuint viewLoc = shader.getUniformLocation(VIEW_MATRIX_UNIFORM_NAME);
		GLuint projectionLoc = shader.getUniformLocation(PROJECTION_MATRIX_UNIFORM_NAME);
		GLuint colorLoc = shader.getUniformLocation(COLOR_UNIFORM_NAME);

		//view matrix
		glm::mat4 view;
		view = application->camera->getViewMarixAfterMoving();
		//projection matrix
		glm::mat4 projection = application->camera->getProjectionMatrix();

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

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
					setMaterialUniform(shape->getMaterial(), shader.program);
					//Draw Call
					glDrawArrays(GL_TRIANGLES, 0, shape->getVertexNumber());
				}
			}
		}

		glBindVertexArray(0);
		
		//Update particles
		application->pworld->runPhysics(deltaTime);
		modelLoc = shader.getUniformLocation(MODEL_MATRIX_UNIFORM_NAME);
		colorLoc = shader.getUniformLocation(COLOR_UNIFORM_NAME);
		//Draw Particles
		Explosion::program = shader.program;
		Explosion::modelLoc = modelLoc;	

		for (crystal::PEffectPtr pe : application->pworld->particleEffects)
		{
			pe->drawEffect(deltaTime);
		}

		//Draw Skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.useShader();
		glm::mat4 skyBoxview = glm::mat4(glm::mat3(application->camera->getViewMarixAfterMoving()));	// Remove any translation component of the view matrix
		glm::mat4 skyBoxProjection = application->camera->getProjectionMatrix();
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.program, "view"), 1, GL_FALSE, glm::value_ptr(skyBoxview));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(skyBoxProjection));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, application->skybox.getCubeTexture());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		if (application->postEffect)
		{
			//Some postEffect is enabled
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			application->postEffect->useShader();
			glBindVertexArray(quadVAO);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}

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
	glfwTerminate();

	return 0;
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
