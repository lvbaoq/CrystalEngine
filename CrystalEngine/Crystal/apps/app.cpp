#include <app\app.h>
#include <iostream>

using namespace crystal;

const char* Application::getTitle()
{
	return "Crystal Engine Application";
}

void Application::initGraphics(GLFWwindow* window)
{
	glWindow = window;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
}

crystal::Vector3 Application::getVector3(const glm::vec3& glmVec)
{
	return Vector3(glmVec.x, glmVec.y, glmVec.z);
}

void Application::start(){}

void Application::display(){}

void Application::update(float deltaTime){}

void Application::key(int keyCode, int action){}

void Application::resize(int width, int height){}

void Application::mouseButton(int button, int action){}

void Application::mouseMove(float x, float y) {}

void Application::scroll(float xoffset, float yoffset){}



Box* createBox(crystal::Vector3 position, crystal::Vector3 halfSize,
	crystal::Material m,bool addCollider,bool canSleep)
{
	Box* box = new Box(Application::globleWorld, position.x, position.y, position.z,
		halfSize.x, halfSize.y, halfSize.z, 
		crystal::Vector3(DEFAULT_WORLD_SIZE, DEFAULT_WORLD_SIZE, DEFAULT_WORLD_SIZE),//The size of the world is not used for now
		m,canSleep);
	CollisionBox* collider = nullptr;
	if (addCollider)
	{
		collider = new CollisionBox();
		collider->halfSize = halfSize;
		collider->body = box;
		collider->calculateInternals();
	}		
	Application::globleWorld->addRigidBody(box,collider);
	return box;
}

Plane* createPlane(crystal::Vector3 planeNormal, crystal::Vector3 position,
	crystal::Material m,bool addCollider,bool canSleep,float drawSizeX, float drawSizeY)
{
	Plane* plane = new Plane(Application::globleWorld, planeNormal, position.x, position.y, position.z, m, drawSizeX, drawSizeY);
	CollisionPlane* collider = nullptr;
	if (addCollider)
	{
		//collider = plane->collider;
		collider = new CollisionPlane();
		collider->direction = plane->normal;
		collider->offset = 0.0f;
		collider->body = plane;
	}
	Application::globleWorld->addRigidBody(plane, collider);
	return plane;
}

Explosion* createExplosion(unsigned num,
	float duration, float maxSpeed, float minSpeed, float gravity, crystal::Material m)
{
	Explosion* e = new Explosion(Application::globlePWorld,num, duration, maxSpeed, minSpeed, gravity, m);
	return e;
}

crystal::ParticleWorld* Application::globlePWorld = nullptr;
crystal:: World* Application::globleWorld = nullptr;