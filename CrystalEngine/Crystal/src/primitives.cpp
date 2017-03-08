#include <app\primitives.h>
#include <app\glm\gtc\type_ptr.inl>

GLuint Box::boxVAO = 0;
GLuint Plane::planeVAO = 0;
GLuint Explosion::particleVAO = 0;
GLuint Explosion::program = 0;
GLuint Explosion::modelLoc = 0;

//Pre-set materials
const Material Material::defaultMaterial(glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f), 0.5f);

const Material Material::emerald(glm::vec3(0.022f,0.174f,0.022f),
	glm::vec3(0.076f,0.614f,0.076f), glm::vec3(0.633f,0.728f,0.633f),0.6f);

const Material Material::jade(glm::vec3(0.135f, 0.223f, 0.158f),
	glm::vec3(0.54f, 0.89f, 0.63f), glm::vec3(0.316f, 0.316f, 0.316f), 0.1f);

const Material Material::obsidian(glm::vec3(0.054f, 0.05f, 0.066f),
	glm::vec3(0.183f, 0.17f, 0.23f), glm::vec3(0.33f, 0.33f, 0.35), 0.3f);

const Material Material::pearl(glm::vec3(0.25f, 0.207f, 0.021f),
	glm::vec3(1.0f, 0.829f, 0.829f), glm::vec3(0.297f, 0.297f, 0.297f), 0.088f);

const Material Material::ruby(glm::vec3(0.175f, 0.012f, 0.012f),
	glm::vec3(0.614f, 0.041f, 0.041f), glm::vec3(0.728f, 0.627f, 0.627f), 0.6f);

Material Material::pureColorMaterial(glm::vec3 color)
{
	Material m(color, color, color, 0.5f);
	return m;
}

glm::mat4 Box::getModelMatrix()
{
	glm::mat4 model;
	/**
	*Transform the original box vertices into this perticular box;
	*We first do scaling operations, then rotations and lastly translations.
	*Otherwise they may affect each other.
	*Since the matrix transform is performed in reversed order,
	*We calculate translation matrix first,then rotation and lastly scaling
	*/
	//Translation
	glm::vec3 glPosition(position.x, position.y, position.z);
	model = glm::translate(model, glPosition);
	//Rotate the matrix.
	if (!orientation.noRoatation())
	{
		float rAngel = 2 * real_acos(orientation.r);
		float aX = orientation.i / real_sqrt(1 - orientation.r*orientation.r);
		float aY = orientation.j / real_sqrt(1 - orientation.r*orientation.r);
		float aZ = orientation.k / real_sqrt(1 - orientation.r*orientation.r);
		glm::vec3 glRotation(aX, aY, aZ);
		model = glm::rotate(model, rAngel, glRotation);
	}

	//Scale the matrix
	glm::vec3 glScale(halfSize.x * 2, halfSize.y * 2, halfSize.z * 2);
	model = glm::scale(model, glScale);

	return model;
}

glm::mat4 Plane::getModelMatrix()
{
	glm::mat4 model;
	//Translation
	//glm::vec3 glPosition(position.x, position.y - (PLANE_THICKNESS / 2.0f), position.z);
	glm::vec3 glPosition(position.x, position.y, position.z);
	model = glm::translate(model, glPosition);
	//Rotate the matrix according to plane normal
	model = glm::rotate(model,crystal::radians(90), glm::vec3(1.0f, 0.0f, 0.0f));

	//Scale the matrix
	glm::vec3 glScale(drawSizeX*10, drawSizeX * 10,1.0f);
	model = glm::scale(model, glScale);

	return model;
}

Explosion::Explosion(crystal::ParticleWorld* pworld,unsigned num,
	float duration, float maxSpeed, float minSpeed, float gravity,Material m) :
	num(num), duration(duration), maxSpeed(maxSpeed), minSpeed(minSpeed),gravity(gravity),material(m)
{
	this->pworld = pworld;
	isPlaying = false;
	destroyable = false;
	m = Material::ruby;//Default material
}

void Explosion::init(crystal::Vector3 position)
{
	if (isPlaying)
	{
		return;
	}
	this->position = position;
	//Set up each particles
	particles.clear();

	for (int i = 0; i < num; i++)
	{
		crystal::Particle* p = new crystal::Particle();
		p->setAcceleration(0.0f, gravity, 0.0f);
		crystal::Vector3 direction(crystal::Random::getRandom(1.0f),
		crystal::Random::getRandom(1.0f), crystal::Random::getRandom(1.0f));
		direction.normalize();
	
		p->setVelocity(direction * crystal::Random::getRandom(minSpeed,maxSpeed));
		p->setPosition(position);
		particles.push_back(p);
	}

	pworld->addParticleEffect(this);
}

void Explosion::drawEffect(float deltaTime)
{
	//Hasn't started playing yet
	if (!isPlaying) return;

	//First time draw
	runTime += deltaTime;

	if (runTime > duration)
	{
		//Played for enough time
		isPlaying = false;
		destroyable = true;
		return;
	}

	glBindVertexArray(particleVAO);
	for (crystal::Particle* p : particles)
	{
		//generate model matrix
		glm::mat4 model;
		//Translation
		model = glm::translate(model, glm::vec3(p->getPosition().x, p->getPosition().y, p->getPosition().z));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Material
		material.setMaterialUniform(program);
		//Draw
		glDrawArrays(GL_TRIANGLES, 0, PARTICLE_VERTEX_NUMBER);
	}
	glBindVertexArray(0);
}

void Explosion::play()
{
	isPlaying = true;
	runTime = 0;
}

Explosion::~Explosion()
{
	for (crystal::Particle* p : particles)
	{
		delete p;
	}
	particles.clear();
}

void DirectionalLight::setLightUniform(GLuint program)
{
	glUniform3f(glGetUniformLocation(program, "dirLight.direction"), direction.x, direction.y, direction.z);
	glUniform3f(glGetUniformLocation(program, "dirLight.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(program, "dirLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(program, "dirLight.specular"), specular.x, specular.y, specular.z);
}

void DirectionalLight::setViewPosition(float x, float y, float z)
{
	GLint viewPosLoc = glGetUniformLocation(Explosion::program, "viewPos");//Bad code here. Need refactor
	glUniform3f(viewPosLoc,x,y,z);
}

void Material::setMaterialUniform(GLuint program)
{
	GLint matAmbientLoc = glGetUniformLocation(program, "material.ambient");
	GLint matDiffuseLoc = glGetUniformLocation(program, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(program, "material.specular");
	GLint matShineLoc = glGetUniformLocation(program, "material.shininess");

	glUniform3f(matAmbientLoc,ambient.x,ambient.y,ambient.z);
	glUniform3f(matDiffuseLoc, diffuse.x,diffuse.y,diffuse.z);
	glUniform3f(matSpecularLoc, specular.x,specular.y,specular.z);
	glUniform1f(matShineLoc,shininess);
}