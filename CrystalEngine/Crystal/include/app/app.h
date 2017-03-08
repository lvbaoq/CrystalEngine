#pragma once
//GL
//Glew
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // !GLEW_STATIC

#include <app\GL\glew.h>
//Glfw
#include <app\GLFW\glfw3.h>

//Camera
#include <app\camera.h>
//Primitive
#include <app\primitives.h>

//Crystal Engine
#include <crystal\crystal.h>

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600
#define DEFAULT_CLEAR_COLOR glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
#define DEFAULT_MAX_CONTACT_NUM 20
#define DEFAULT_MAX_PARTICLE_CONTACT_NUM 50
#define DEFAULT_WORLD_SIZE 10
#define DEFAULT_CONTACT_RESOLVE_ITERATION 8

class Application
{
public:
	//Screen width & height
	int screenWidth;
	int screenHeight;
	//Screen ratio
	float ratio;
	//Camera object
	Camera* camera;
	//Directional lights
	DirectionalLight* dirLight;
	//3d world object
	crystal::World* world;
	//Particle world object
	crystal::ParticleWorld* pworld;
	//Window object of glfw
	GLFWwindow* glWindow;
	//Background color. Used to clear screen at each frame
	glm::vec4 clearColor = DEFAULT_CLEAR_COLOR;
	//Enable physics simulation
	bool runPhysics;
	//Pause game loop
	bool pause;

	crystal::Vector3 worldSize;

	Application(int width = DEFAULT_SCREEN_WIDTH,int height = DEFAULT_SCREEN_HEIGHT,
		int maxContactNum = DEFAULT_MAX_CONTACT_NUM,
		int maxPContactNum = DEFAULT_MAX_PARTICLE_CONTACT_NUM,
		float worldMaxX = DEFAULT_WORLD_SIZE,
		float worldMaxY = DEFAULT_WORLD_SIZE,
		float worldMaxZ = DEFAULT_WORLD_SIZE):
		screenWidth(width),screenHeight(height),pause(false)
	{		
		runPhysics = true;
		ratio = (float)width /(float)height;
		world = new crystal::World(maxContactNum, DEFAULT_CONTACT_RESOLVE_ITERATION);
		pworld = new crystal::ParticleWorld(maxPContactNum);
		worldSize = crystal::Vector3(worldMaxX, worldMaxY, worldMaxZ);
		camera = new Camera{ ratio };
		dirLight = NULL;
	}

	~Application()
	{
		delete dirLight;
		delete camera;
		delete world;
	}

	static crystal::Vector3 getVector3(const glm::vec3& glmVec);

	/**
	* Called before main loop starts
	* Used to init scenes
	* The default implementation does nothing
	*/
	virtual void start();

	/**
	* Gets the title of the demo for the title bar of the window.
	*
	* The default implementation returns a generic title.
	*/
	virtual const char* getTitle();

	/**
	* Sets up the graphics, and allows the application to acquire
	* graphical resources. Guaranteed to be called after OpenGL is
	* set up.
	*
	* @param window The window object to manipulate
	*
	* The default implementation sets up a basic view
	*/
	virtual void initGraphics(GLFWwindow* window);

	/**
	* Called each frame to display the current scene. The common code
	* will automatically flush the graphics pipe and swap the render
	* buffers after calling this so flush method doesn't need to be called.
	*
	* The default
	* implementation draws a simple diagonal line across the surface
	* (as a sanity check to make sure GL is working).
	*/
	virtual void display();

	/**
	* Called each frame to update the current state of the scene.
	*
	* The default implementation does nothing
	*/
	virtual void update(float deltaTime);

	/**
	* Called when a keypress is detected.
	*
	* The default implementation does nothing.
	*
	* @param keyCode The code of the key represented by glfw.
	* @param action The action of key (GLFW_PRESS or GLFW_RELEASE)
	*/
	virtual void key(int keyCode,int action);

	/**
	* Notifies the application that the window has changed size.
	* The new size is given.
	*
	* The default implementation sets the internal height and width
	* parameters and changes the gl viewport. These are steps you'll
	* almost always need, so its worth calling the base class version
	* of this method even if you override it in a demo class.
	*/
	virtual void resize(int width, int height);

	/**
	* Called when GLUT detects a mouse button press.
	*
	* The default implementation does nothing.
	*/
	virtual void mouseButton(int button, int action);

	/**
	* Called when mouse moves
	*
	* The default implementation does nothing.
	*/
	virtual void mouseMove(float x, float y);

	/**
	* Called when mouse scrolls
	* The default implementation does nothing.
	*/
	virtual void scroll(float xoffset, float yoffset);

	// These are helper functions that can be used by an application
	// to render things.

	/**
	* Renders the given text to the given x,y location (in screen space)
	* on the window. This is used to pass status information to the
	* application.
	*/
	void renderText(float x, float y, const char *text, void* font = NULL);


};