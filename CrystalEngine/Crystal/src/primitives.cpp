#include <app\primitives.h>

GLuint Box::boxVAO = 0;
GLuint Plane::planeVAO = 0;
GLuint Explosion::particleVAO = 0;
GLuint Explosion::program = 0;
GLuint Explosion::modelLoc = 0;

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
	glm::vec3 glScale(scale*drawSizeX, scale*drawSizeX,1.0f);
	model = glm::scale(model, glScale);

	return model;
}

Explosion::Explosion(crystal::ParticleWorld* pworld,unsigned num,
	float duration, float maxSpeed, float minSpeed, float gravity, crystal::Material m) :
	num(num), duration(duration), maxSpeed(maxSpeed), minSpeed(minSpeed),gravity(gravity),material(m)
{
	this->pworld = pworld;
	isPlaying = false;
	destroyable = false;
	m = crystal::Material::ruby;//Default material
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
		particles.emplace_back(p);
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
	for (crystal::ParticlePtr p : particles)
	{
		//generate model matrix
		glm::mat4 model;
		//Translation
		model = glm::translate(model, glm::vec3(p->getPosition().x, p->getPosition().y, p->getPosition().z));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Material
		setMaterialUniform(material,program);
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
	particles.clear();
}

glm::mat4 DirectionalLight::getLightSpaceMatrix()
{
	glm::mat4 projection, view, space;
	projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop,near_plane,far_plane);
	view = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	space = projection * view;
	return space;
}

void DirectionalLight::setLightUniform(GLuint program)
{
	GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
	glUniform3f(viewPosLoc, viewPostion.x, viewPostion.y, viewPostion.z);

	glUniform3f(glGetUniformLocation(program, "dirLight.position"), position.x, position.y, position.z);
	glUniform3f(glGetUniformLocation(program, "dirLight.direction"), direction.x, direction.y, direction.z);
	glUniform3f(glGetUniformLocation(program, "dirLight.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(program, "dirLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(program, "dirLight.specular"), specular.x, specular.y, specular.z);

	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(getLightSpaceMatrix()));
}

void DirectionalLight::setViewPosition(float x, float y, float z)
{
	viewPostion.x = x;
	viewPostion.y = y;
	viewPostion.z = z;
}

//Helper functions
void setMaterialUniform(crystal::Material& m, GLuint program)
{
	GLint matAmbientLoc = glGetUniformLocation(program, "material.ambientColor");
	GLint matDiffuseLoc = glGetUniformLocation(program, "material.diffuseColor");
	GLint matSpecularLoc = glGetUniformLocation(program, "material.specularColor");
	GLint matShineLoc = glGetUniformLocation(program, "material.shininess");

	glUniform3f(matAmbientLoc, m.ambient.x, m.ambient.y, m.ambient.z);
	glUniform3f(matDiffuseLoc, m.diffuse.x, m.diffuse.y, m.diffuse.z);
	glUniform3f(matSpecularLoc, m.specular.x, m.specular.y, m.specular.z);
	glUniform1f(matShineLoc, m.shininess);

	//Init unset textures to default texture
	m.initTextures();

	//Bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m.diffuseMap.getTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m.specularMap.getTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m.emissionMap.getTexture());
}