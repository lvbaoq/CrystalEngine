#include <app\app.h>

/**
 * A sample application of using Crystal Engine
 */

using namespace crystal;

/*Declaration of callback methods when collision occurs*/
void bulletOnCollision(World* world, CollisionPrimitive* bullet, CollisionPrimitive* other);
void boxOnCollision(World* world, CollisionPrimitive* bullet, CollisionPrimitive* other);
/* Holds a reference to the ParticleWorld object. Can be used in collision callback methods */
static ParticleWorld* pWorld;

/*A first-person game. Can walk around and shoot some blocks*/
class BlockShooter:public Application
{
private:
	//Application-specific attributes
	//All the attributes below is usd to mock a jump effect
	bool isJumping = false;
	float jumpSpeed = 3.0f;
	float gravity = 9.8f;
	float jumpTime;
	float cameraYPosition;
public:
	
	/* This method is called before the game loop starts. It is used to do some initing work */
	void start()
	{
		//Init globle particle world reference
		pWorld = pworld;

		//Set directional light
		dirLight = new DirectionalLight();
		dirLight->direction = glm::vec3(0.0f,-3.0f,-1.0f);
		dirLight->setViewPosition(camera->position.x,camera->position.y,camera->position.z+5.0f);

		//Place ground
		Plane* plane = new Plane(world,crystal::Vector3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 0.0f,
			Material::pureColorMaterial(glm::vec3(0.45f,0.29f,0.07f)));//A brown ground
		world->addRigidBody(plane, plane->collider);
		
		//Place some blocks to shoot
		Box* box1 = new Box(world,0.0f,5.0f,0.0f,1.0f,1.0f,1.0f,worldSize,Material::obsidian);
		box1->setAcceleration(0.0f,-1.0f,0.0f);
		//Set rotation
		//box1->setOrientation(Quaternion(60.0f, Vector3(1.0f, 0.0f, 0.0f)));
		//Add the rigidbody and its attached collider to the scene
		world->addRigidBody(box1, box1->collider);
		//Add a callback method for the collider. This method is automatically invoked when collision happens
		world->addCallbackMethod(box1->collider, boxOnCollision);

		Box* box2 = new Box(world, 4.0f, 6.0f, 0.0f, 1.0f, 1.0f, 1.0f, worldSize,Material::pearl);
		box2->setVelocity(2.0f,0.0f,-1.0f);
		box2->setAcceleration(0.0f, -1.0f, 0.0f);
		world->addRigidBody(box2, box2->collider);
		world->addCallbackMethod(box2->collider, boxOnCollision);
		//Add a constant force
		//ForceGenerator* f = new ConstantForce(Vector3(1.5f, 1.5f, 0.0f));
		//world->forceRegistry.add(box, f);
	}

	/** 
	 * This method is called every frame before update happens
	 * You can use this method to display some visual effect by invoking some opengl functions 
	 */
	void display()
	{
		
	}

	/* This is the main update function called every frame */
	void update(float deltaTime)
	{
		//Move camera
		camera->moveOnXZPlain(5.0f*deltaTime);
		//Jumping
		if (isJumping)
		{
			//Manually move the camera to simulate a jump effect
			float timeInAir = glfwGetTime() - jumpTime;
			float yPosition = cameraYPosition + jumpSpeed*timeInAir - 0.5f*gravity*timeInAir*timeInAir;
			if (yPosition <= cameraYPosition)
			{
				//Land
				yPosition = cameraYPosition;
				isJumping = false;
			}
			//Set camera position
			camera->position.y = yPosition;
		}
	}
	/* Display a title on the window */
	const char* getTitle()
	{
		return "Block Shooter";
	}

	//This method is called before game loop starts
	void initGraphics(GLFWwindow* window)
	{
		//Set graphics options
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//No resize
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//Hide cursor

		//Call parent methods to finish initialization
		Application::initGraphics(window);
	}

	//Call back functions
	void key(int keyCode, int action)
	{
		if (keyCode == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(glWindow, GLFW_TRUE);
		}
		//Pause Game
		if (keyCode == GLFW_KEY_P && action == GLFW_PRESS)
		{
			pause = !pause;
		}
		camera->keyCallBack(keyCode, action);
		if (keyCode == GLFW_KEY_SPACE && !isJumping)
		{
			isJumping = true;
			jumpTime = glfwGetTime();
			cameraYPosition = camera->position.y;
		}

	}

	void mouseButton(int button, int action)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (action == GLFW_PRESS)
				camera->zoom();
			else if (action == GLFW_RELEASE)
				camera->defaultFieldOfView();
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (action == GLFW_RELEASE) 
			{
				crystal::Vector3 cameraFront = Application::getVector3(camera->getFrontVec());
				//shoot a block
				Box* bullet = new Box(world,camera->position.x + cameraFront.x*0.5f,
					camera->position.y, camera->position.z + cameraFront.z*0.5f,
					0.1f, 0.1f, 0.1f, worldSize,Material::ruby);
				bullet->setAcceleration(0.0f, -1.0f, 0.0f);
				bullet->setVelocity(cameraFront*3.0f);
				bullet->tag = "Bullet";
				world->addRigidBody(bullet, bullet->collider);
				world->addCallbackMethod(bullet->collider, bulletOnCollision);
			}		
		}
	}

	void mouseMove(float x, float y)
	{
		camera->mouseCallBack(x, y);
	}

	void scroll(float xoffset, float yoffset)
	{
		camera->scrollToZoom(yoffset);
	}

};

void bulletOnCollision(World* world, CollisionPrimitive* bullet, CollisionPrimitive* other)
{
	world->deleteBody(bullet->body);
}

void boxOnCollision(World* world, CollisionPrimitive* box, CollisionPrimitive* other)
{
	if (other->body->tag == "Bullet")//Every body has a 'tag' attribute
	{
		world->deleteBody(box->body);
		//Play some particle effects
		Explosion* e = new Explosion(pWorld, 20, 2.0f, 2.0f, 1.0f, -1.0f,
			Material::pureColorMaterial(glm::vec3(0.8f,0.8f,0.1f)));
		e->init(other->body->getPosition());
		e->play();
	}	
}
/* This method is used to tell the main method which application to use */
Application* getApplication()
{
	return new BlockShooter();
}