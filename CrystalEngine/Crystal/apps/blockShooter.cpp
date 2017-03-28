#include <app\app.h>

/**
* A sample application of using Crystal Engine
*/

using namespace crystal;

/*Declaration of callback methods when collision occurs*/
void bulletOnCollision(World* world, CollisionPrimitive* bullet, CollisionPrimitive* other);
void boxOnCollision(World* world, CollisionPrimitive* bullet, CollisionPrimitive* other);


/*A first-person game. Can walk around and shoot some blocks*/
class BlockShooter :public Application
{
private:
	//Application-specific attributes
	//All the attributes below is usd to mock a jump effect
	bool isJumping = false;
	float jumpSpeed = 3.0f;
	float gravity = 9.8f;
	float jumpTime;
	float cameraYPosition;

	unsigned sampleNum = 4;
public:

	/* This method is called before the game loop starts. It is used to do some initing work */
	void start()
	{
		//Set directional light
		dirLight->direction = glm::vec3(0.0f, -1.0f, -1.0f);
		dirLight->setViewPosition(camera->position.x, camera->position.y, camera->position.z);
		dirLight->position = glm::vec3(-3.0f, 6.0f, 5.0f);
		Box* po = createBox(Vector3(-3.0f, 5.0f, 5.0f), Vector3(0.2f, 0.2f, 0.2f));
		po->setAcceleration(0.0f, 0.0f, 0.0f);
		//dirLight->diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
		//dirLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		//dirLight->specular = glm::vec3(0.0f, 0.0f, 0.0f);

		//Place ground		
		Plane* plane = createPlane(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f),
			Material::pureColorMaterial(Vector3(0.45f, 0.29f, 0.07f)));//A brown ground
		plane->getMaterial().setDiffuseMap("./Assets/Textures/ground.jpg");

		//Place some blocks to shoot
		Box* box1  = createBox(Vector3(0.0f, 5.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));	
		box1->getMaterial().setDiffuseMap("./Assets/Textures/container.png");
		box1->getMaterial().setSpecularMap("./Assets/Textures/container_specular.png");
		box1->getMaterial().setEmissionMap("./Assets/Textures/matrix.jpg");
		box1->setAcceleration(0.0f, -1.0f, 0.0f);
		//Set rotation
		//box1->setOrientation(Quaternion(60.0f, Vector3(1.0f, 0.0f, 0.0f)));
		//Add a callback method for the collider. This method is automatically invoked when collision happens
		world->addCallbackMethod(box1, boxOnCollision);

		Box* box2 = createBox(Vector3(4.0f, 6.0f, 0.0f),Vector3(1.0f, 1.0f, 1.0f));
		box2->getMaterial().setDiffuseMap("./Assets/Textures/container.png");
		box2->getMaterial().setSpecularMap("./Assets/Textures/container_specular.png");
		box2->getMaterial().setEmissionMap("./Assets/Textures/matrix.jpg");
		box2->setVelocity(2.0f, 0.0f, -1.0f);
		box2->setAcceleration(0.0f, -1.0f, 0.0f);
		world->addCallbackMethod(box2, boxOnCollision);
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
		dirLight->setViewPosition(camera->position.x, camera->position.y, camera->position.z);
	}
	/* Display a title on the window */
	const char* getTitle()
	{
		return "Block Shooter";
	}

	void initGraphicSettings()
	{
		//Some settings to take effect before creating gl window 
		MSAASampleNum = sampleNum;
	}

	//This method is called before game loop starts
	void initGraphics(GLFWwindow* window)
	{
		//Set graphics options
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//No resize
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//Hide cursor

		//Set MSAA sample number
		MSAASampleNum = sampleNum;

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
		//Enable some post effects
		if (keyCode == GLFW_KEY_1 && action == GLFW_PRESS)
		{
			//Inversion
			postEffect = &PostProcessing::Inversion;
		}

		if (keyCode == GLFW_KEY_2 && action == GLFW_PRESS)
		{
			//Grayscale
			postEffect = &PostProcessing::Grayscale;
		}

		if (keyCode == GLFW_KEY_3 && action == GLFW_PRESS)
		{
			//Blur
			postEffect = &PostProcessing::Blur;
		}

		if (keyCode == GLFW_KEY_4 && action == GLFW_PRESS)
		{
			//Sharpen
			postEffect = &PostProcessing::Sharpen;
		}
		if (keyCode == GLFW_KEY_5 && action == GLFW_PRESS)
		{
			//Edge Detection
			postEffect = &PostProcessing::EdgeDetection;
		}
		if (keyCode == GLFW_KEY_0 && action == GLFW_PRESS)
		{
			//Back to normal
			postEffect = nullptr;
		}
		if (keyCode == GLFW_KEY_G && action == GLFW_PRESS)
		{
			//Enable or disable gamma correction
			useGammaCorrection = !useGammaCorrection;
		}

		if (keyCode == GLFW_KEY_R && action == GLFW_PRESS)
		{
			dirLight->diffuse += 0.2;
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
				Box* bullet = createBox(Vector3(camera->position.x + cameraFront.x*0.5f,
					camera->position.y, camera->position.z + cameraFront.z*0.5f),
					Vector3(0.1f, 0.1f, 0.1f), Material::ruby);
				bullet->setAcceleration(0.0f, -1.0f, 0.0f);
				bullet->setVelocity(cameraFront*3.0f);
				bullet->tag = "Bullet";
				world->addCallbackMethod(bullet, bulletOnCollision);
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
		Explosion* e = createExplosion(20, 2.0f, 2.0f, 1.0f, -1.0f,
			Material::pureColorMaterial(Vector3(0.8f, 0.8f, 0.1f)));
		e->init(other->body->getPosition());
		e->play();
	}
}
/* This method is used to tell the main method which application to use */
std::unique_ptr<Application> getApplication()
{
	return std::unique_ptr<Application>(new BlockShooter());
}