#include <app\app.h>

using namespace crystal;

class TreePlant :public Application
{
private:
	int sampleNum = 4;
public:
	void start()
	{
		//Set directional light
		dirLight->direction = glm::vec3(0.0f, -1.0f, -1.0f);
		dirLight->setViewPosition(camera->position.x, camera->position.y, camera->position.z);
		dirLight->position = glm::vec3(-3.0f, 6.0f, 5.0f);

		//Place ground		
		Plane* plane = createPlane(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f),
			Material::pureColorMaterial(Vector3(0.45f, 0.29f, 0.07f)));//A brown ground
		plane->getMaterial().setDiffuseMap("./Assets/Textures/ground.jpg");
		plane->scale = 5;
		//Load Models
		//Model model("./Assets/Models/nanosuit/nanosuit.obj");
		//int modelId = createModel("./Assets/Models/nanosuit/nanosuit.obj");
		int modelId = createModel("./Assets/Models/Needle/needle01.obj");
		Model& m = getModel(modelId);
		m.drawModel = false;
		m.position = Vector3(0.0f, 0.02f, 0.0f);
		m.scale = Vector3(0.6f, 0.6f, 0.6f);
		//Massive plant
		areaInstancing(modelId, 0.02f, -30.0f, -30.0f, 60.0f, 60.0f, 10, 10, 0.2f, 0.5f);
	}

	/* This is the main update function called every frame */
	void update(float deltaTime)
	{
		//Move camera
		camera->move(5.0f*deltaTime);
		dirLight->setViewPosition(camera->position.x, camera->position.y, camera->position.z);
	}

	/* Display a title on the window */
	const char* getTitle()
	{
		return "Tree Plant";
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

		camera->keyCallBack(keyCode, action);
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

/* This method is used to tell the main method which application to use */
std::unique_ptr<Application> getApplicationTp()
{
	return std::unique_ptr<Application>(new TreePlant());
}