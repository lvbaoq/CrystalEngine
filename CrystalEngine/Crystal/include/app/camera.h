#pragma once
//Glew
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // !GLEW_STATIC
#include "GL\glew.h"
//GLFW
#include "GLFW\glfw3.h"

#include "primitives.h"

#define DEFAULT_ZOOMED_FOV 44.7f
#define DEFAULT_FOV 45.0f

class Camera
{
private:	
	glm::vec3 front;//The direction the camera looks. (reversal of z axis)
	glm::vec3 right;//x axis
	glm::vec3 up;//y axis
	glm::mat4 viewMatrix;
	
	glm::vec3 worldUp, target;

	//Field of view (in degrees)
	GLfloat fov = DEFAULT_FOV;
	GLfloat MAX_FOV = 45.0f;
	GLfloat MIN_FOV = 1.0f;
	GLfloat scrollSensitivity = 0.05f;

	//Track mouse position
	GLfloat lastX = 400, lastY = 300;
	GLfloat mouseSensitivity = 0.05f;
	bool firstMouse = true;//Mark the first position of mouse to prevent sudden jump when starting up

	//Used for rotation with mouse
	GLfloat yaw = -90.0f;// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work
	GLfloat pitch = 0.0f;
	GLfloat PITCH_UP_BOUND = 89.0f;
	GLfloat PITCH_DOWN_BOUND = -89.0f;

	//A set of keys to move the camera around
	int FORWARD_KEY = GLFW_KEY_W;
	int BACKWARD_KEY = GLFW_KEY_S;
	int LEFT_KEY = GLFW_KEY_A;
	int RIGHT_KEY = GLFW_KEY_D;

	bool keys[1024];

	GLfloat projectionRatio;

public:
	glm::vec3 position;
	float nearClip = 0.1f;
	float farClip = 100.0f;

	Camera(float projectionRatio, glm::vec3 cPosition = glm::vec3(0.0f, 1.0f, 5.0f),
		glm::vec3 target = glm::vec3(0.0f,0.0f,-1.0f),
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f)):
		projectionRatio(projectionRatio),position(cPosition),target(target),worldUp(worldUp)
	{
		glm::vec3 zAxis = glm::normalize(position - target);
		worldUp = glm::normalize(worldUp);
		right = glm::cross(worldUp, zAxis);
		up = glm::cross(zAxis, right);
		front = -zAxis;
		for (int i = 0; i < 1024; i++)
		{
			keys[i] = false;
		}
	}

	glm::mat4 getProjectionMatrix()
	{
		glm::mat4 projection = glm::perspective(getFieldOfView(),
			projectionRatio, nearClip, farClip);
		return projection;
	}

	glm::mat4 getViewMarixAfterMoving()
	{
		viewMatrix = glm::lookAt(position, position+front, worldUp);
		return viewMatrix;
	}

	void moveOnXZPlain(float distance)
	{
		//Moves only on X_Z plain
		glm::vec3 xzFront = glm::vec3(front.x, 0, front.z);
		xzFront = glm::normalize(xzFront);
		glm::vec3 xzRight = glm::cross(xzFront, worldUp);

		if (keys[FORWARD_KEY])
		{
			//move forward
			position += xzFront * distance;
		}
		if (keys[BACKWARD_KEY])
		{
			//move backward
			position -= xzFront * distance;
		}
		if (keys[LEFT_KEY])
		{
			//move left
			position -= xzRight * distance;
		}
		if (keys[RIGHT_KEY])
		{
			//move right
			position += xzRight * distance;
		}
	}

	void move(float distance)
	{ 

		if (keys[FORWARD_KEY])
		{
			//move forward
			position += front * distance;
		}
		if (keys[BACKWARD_KEY])
		{
			//move backward
			position -= front * distance;
		}
		if (keys[LEFT_KEY])
		{
			//move left
			position -= right * distance;
		}
		if (keys[RIGHT_KEY])
		{
			//move right
			position += right * distance;
		}
	}

	void mouseCallBack(float xpos,float ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			return;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
		lastX = xpos;
		lastY = ypos;

		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > PITCH_UP_BOUND)
		{
			pitch = PITCH_UP_BOUND;
		}
		else if (pitch < PITCH_DOWN_BOUND)
		{
			pitch = PITCH_DOWN_BOUND;
		}

		//Calculate new front vector
		glm::vec3 newFront;
		newFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		newFront.y = sin(glm::radians(pitch));
		newFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		setFrontVec(newFront);
	}
	
	void keyCallBack(int key, int action)
	{
		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
			{
				keys[key] = true;
			}
			else if (action == GLFW_RELEASE)
			{
				keys[key] = false;
			}
		}
	}

	void scrollToZoom(float offset)
	{
		if (fov >= MIN_FOV && fov <= MAX_FOV)
			fov -= offset*scrollSensitivity;
		checkFovBounds();
	}

	void zoom(GLfloat newFov = DEFAULT_ZOOMED_FOV)
	{
		fov = newFov;
		checkFovBounds();
	}

	void defaultFieldOfView()
	{
		fov = DEFAULT_FOV;
	}

	GLfloat getFieldOfView() const
	{
		return fov;
	}

	glm::vec3 getFrontVec() const
	{
		return front;
	}

	glm::vec3 getRightVec() const
	{
		return right;
	}

	void setFrontVec(const glm::vec3 front)
	{
		this->front = glm::normalize(front);
		right = normalize(glm::cross(worldUp, -front));
		up = glm::cross(-front, right);
	}

	void setPosition(const glm::vec3 newPosition)
	{
		position = newPosition;
	}

	private:
		void checkFovBounds()
		{
			if (fov <= MIN_FOV)
				fov = MIN_FOV;
			if (fov >= MAX_FOV)
				fov = MAX_FOV;
		}

};