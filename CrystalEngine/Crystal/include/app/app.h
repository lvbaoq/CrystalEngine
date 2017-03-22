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
//Graphics
#include <app\graphics.h>
#include <memory>

#ifdef _DEBUG
//#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600
#define DEFAULT_CLEAR_COLOR glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
#define DEFAULT_MAX_CONTACT_NUM 20
#define DEFAULT_MAX_PARTICLE_CONTACT_NUM 50
#define DEFAULT_WORLD_SIZE 100
#define DEFAULT_CONTACT_RESOLVE_ITERATION 8

//Use smart pointers to manage common objects
using DirectionLightPtr = std::unique_ptr<DirectionalLight>;
using CameraPtr = std::unique_ptr<Camera>;
using WorldPtr = std::unique_ptr<crystal::World>;
using ParticleWorldPtr = std::unique_ptr<crystal::ParticleWorld>;

class Application
{
public:

	/* Holds a reference to the ParticleWorld object. Can be used in collision callback methods */
	static crystal::ParticleWorld* globlePWorld;
	static crystal::World* globleWorld;

	//Screen width & height
	int screenWidth;
	int screenHeight;
	//Screen ratio
	float ratio;
	//Graphics settings
	/*The sample number of each fragment for MASS effect. Default is 0. Set a value larger than 0 to enable MSAA*/
	unsigned MSAASampleNum;
	//Camera object
	CameraPtr camera;
	//Directional lights
	DirectionLightPtr dirLight;
	//3d world object
	WorldPtr world;
	//Particle world object
	ParticleWorldPtr pworld;
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
		screenWidth(width),screenHeight(height),pause(false),MSAASampleNum(0)
	{		
		runPhysics = true;
		ratio = (float)width /(float)height;
		world.reset(new crystal::World(maxContactNum, DEFAULT_CONTACT_RESOLVE_ITERATION));
		pworld.reset(new crystal::ParticleWorld(maxPContactNum));
		worldSize = crystal::Vector3(worldMaxX, worldMaxY, worldMaxZ);
		camera.reset(new Camera{ ratio });
		dirLight.reset(new DirectionalLight());
		Application::globleWorld = world.get();
		Application::globlePWorld = pworld.get();
	}

	virtual ~Application() {}

	static crystal::Vector3 getVector3(const glm::vec3& glmVec);

	/**
	 * Set some graphic settings before creating gl window objcet
	 * e.g. Set msaa sample number
	 */
	virtual void initGraphicSettings();

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

/* Some helper functions */
//Factory methods to add primitives to scene
Box* createBox(crystal::Vector3 position, crystal::Vector3 halfSize,
	crystal::Material m = crystal::Material::defaultMaterial,
	bool addCollider = true, bool canSleep = true);

Plane* createPlane(crystal::Vector3 planeNormal, crystal::Vector3 position,
	crystal::Material m = crystal::Material::defaultMaterial, bool addCollider = true, bool canSleep = true,
	float drawSizeX = PLANE_DRAW_SIZE, float drawSizeY = PLANE_DRAW_SIZE);

//Factory methods to add particle effects to the scene
Explosion* createExplosion(unsigned num,
	float duration, float maxSpeed, float minSpeed, float gravity, crystal::Material m);