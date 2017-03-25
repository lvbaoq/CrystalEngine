//Memory leak test
#define _CRTDBG_MAP_ALLOC
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

int main()
{

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

	//Init shaders with lights enabled
	Shader shader{LIGHT_VETEX_SHADER_PATH,LIGHT_FRAGMENT_SHADER_PATH};
	Shader skyboxShader{ DEFAULT_SKYBOX_VERTEX_SHADER_PATH,DEFAULT_SKYBOX_FRAGMENT_SHADER_PATH };

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

	//Set texture units
	shader.useShader();
	glUniform1i(glGetUniformLocation(shader.program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shader.program, "material.specular"), 1);
	glUniform1i(glGetUniformLocation(shader.program, "material.emission"), 2);

	//Set up application
	application->start();

	//Load skybox
	application->skybox.loadCubeMap();

	//Main Game Loop
	while (!glfwWindowShouldClose(window))
	{
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
		//Clear Screen
		glm::vec4 cColor = application->clearColor;
		glClearColor(cColor.x, cColor.y, cColor.z, cColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Update
		application->world->startFrame();
		application->update(deltaTime);
		//Run Physics
		if (application->runPhysics)
		{
			application->world->startFrame();
			application->world->runPhysics(deltaTime);
		}
		//Draw
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

		//Finish draw all objects
		glfwSwapBuffers(window);
	}

	//Properly release gl resources
	glDeleteBuffers(1, &boxVBO);
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteBuffers(1, &squareVBO);
	glDeleteVertexArrays(1, &squareVAO);
	shader.deleteShader();
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
