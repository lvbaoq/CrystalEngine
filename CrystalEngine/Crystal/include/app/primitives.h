#pragma once
#include <crystal\crystal.h>
//Glew
#define GLEW_STATIC
#include "GL\glew.h"
//GLFW
#include "GLFW\glfw3.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.inl"
#include "graphics.h"

#define BOX_VERTEX_NUMBER 36
#define PLANE_VERTEX_NUMBER 6
#define PLANE_DRAW_SIZE 350
#define PARTICLE_VERTEX_NUMBER 6
#define PLANE_THICKNESS 0.01f
#define DEFAULT_COLOR_R 0.2f
#define DEFAULT_COLOR_G 0.5f
#define DEFAULT_COLOR_B 0.3f
#define DEFAULT_COLOR_A 1.0f
#define DEFAULT_DENSITY 1.0f

/* A gloable method to set the material data to the shader's uniform */
void setMaterialUniform(crystal::Material& m, GLuint program);

class Primitive:public crystal::RigidBody
{
protected:
	crystal::Material material;
	crystal::Vector3 worldSize;
	crystal::World* world;
	
public:
	crystal::CollisionPrimitive* collider;
	bool isPrimitive() { return true; };
	//Get the transform matrix to convert the box to world coordinate
	virtual glm::mat4 getModelMatrix() { return glm::mat4(); };
	//Get the vetex array object to draw the primitive
	virtual GLuint getVAO() { return 0; };
	//Get the number of vetex to draw
	virtual GLint getVertexNumber() { return 0; };
	//Get the color of the primitive, in RGBA format
	virtual crystal::Material& getMaterial() { return material; };

	void setMaterial(crystal::Material m) { this->material = m; };

	virtual ~Primitive() {};
};

/* A simple box primitive with a box collider attached to it */
class Box :public Primitive
{

public:
	static GLuint boxVAO;
	crystal::CollisionBox boxCollider;

	crystal::Vector3 halfSize;

	Box(crystal::World* world, crystal::real pX, crystal::real pY, crystal::real pZ,
		crystal::real halfSizeX, crystal::real halfSizeY, crystal::real halfSizeZ,
		crystal::Vector3 wSize, crystal::Material m = crystal::Material::defaultMaterial, bool canSleep = true) :
		halfSize(halfSizeX, halfSizeY, halfSizeZ)
	{
		position = crystal::Vector3(pX, pY, pZ);
		worldSize = wSize;
		//boxCollider = crystal::CollisionBox();
		//boxCollider.halfSize = halfSize;
		//boxCollider.body = this;
		//collider = &boxCollider;
		this->world = world;

		material = m;

		//Set some default values
		//Enable gravity by default
		acceleration = crystal::Vector3::GRAVITY;
		//Set Linear damping
		linearDamping = (crystal::real)0.98f;
		//Set Angular damping
		angularDamping = (crystal::real)0.8f;
		//Set mass according to volume
		setMass(DEFAULT_DENSITY * halfSizeX * halfSizeY * halfSizeZ);
		//Set inertial tensor
		crystal::Matrix3 tensor;
		tensor.setBlockInertiaTensor(halfSize, mass);
		setInertiaTensor(tensor);

		clearAccumulators();
		setOrientation(crystal::Quaternion());
		setRotation(crystal::Vector3(0, 0, 0));
		calculateDerivedData();
		setCanSleep(canSleep);

		//boxCollider.calculateInternals();
	}

	GLint getVertexNumber()
	{
		return BOX_VERTEX_NUMBER;
	}

	GLuint getVAO()
	{
		return boxVAO;
	}

	glm::mat4 getModelMatrix();
};

/* A plane, which has infinit size*/
class Plane :public Primitive
{
public:
	static GLuint planeVAO;
	int drawSizeX;
	int drawSizeY;
	crystal::CollisionPlane planeCollider;
	crystal::Vector3 normal;

	Plane(crystal::World* world, crystal::Vector3 planeNormal,
		crystal::real pX, crystal::real pY, crystal::real pZ,
		crystal::Material m = crystal::Material::defaultMaterial,
		crystal::real drawSizeX = PLANE_DRAW_SIZE, crystal::real drawSizeY = PLANE_DRAW_SIZE) :
		drawSizeX(drawSizeX),drawSizeY(drawSizeY)
	{
		position = crystal::Vector3(pX, pY, pZ);
		normal = planeNormal;
		material = m;
		setInverseMass(1.0f);
		
		//Set up rigid body
		clearAccumulators();
		acceleration = crystal::Vector3(0, 0, 0);

	}

	GLint getVertexNumber()
	{
		return PLANE_VERTEX_NUMBER;
	}

	GLuint getVAO()
	{
		return Plane::planeVAO;
	}

	glm::mat4 getModelMatrix();
};

/** 
 * An explosion effect
 * Create some particles with a random speed between @maxSpeed and @minSpeed. 
 * The moving direction of a particle is random in a sphere plane
 * The color of a particle can be set to a specific color or random pick around it
 */
class Explosion :public crystal::ParticleEffect
{
private:
	float runTime;
public:

	/* Necessary info to draw particles */
	static GLuint particleVAO;
	static GLuint program;
	static GLuint modelLoc;

	unsigned num;//Number of particles
	float duration;//The duration of particles
	float maxSpeed;//Max speed of particles
	float minSpeed;//Min speed of particles
	float gravity;//Gravity size
	crystal::Material material;

	Explosion(crystal::ParticleWorld* pworld,unsigned num, float duration, 
		float maxSpeed, float minSpeed, float gravity = defalut_gravity, crystal::Material m = crystal::Material::ruby);
	void play();
	void init(crystal::Vector3 position);
	void drawEffect(float deltaTime);
	~Explosion();
};

/* Representing a directional light in the scene */
class DirectionalLight
{
public:
	glm::vec3 direction;

	glm::vec3 ambient;//Ambient color of the light
	glm::vec3 diffuse;//diffuse color of the light
	glm::vec3 specular;//specular color of the light

	DirectionalLight()
	{
		//Set default direction and color
		direction = glm::normalize(glm::vec3(0.0f,-1.0f,1.0f));
		ambient = diffuse = specular = glm::vec3(1.0f, 1.0f, 1.0f);//White color
	}

	DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):
		direction(direction),ambient(ambient),diffuse(diffuse),specular(specular)
	{}

	void setLightUniform(GLuint program);

	void setViewPosition(float x,float y,float z);
};

