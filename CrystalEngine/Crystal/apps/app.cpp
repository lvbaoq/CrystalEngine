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

