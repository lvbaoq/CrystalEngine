#include <assert.h>
#include <crystal/particle.h>

using namespace crystal;

Particle::Particle():inverseMass(1.0f), damping(1.0f)
{

}

void Particle::setMass(const real mass)
{
	assert(mass != 0);
	inverseMass = ((real)1.0) / mass;
}

void Particle::setInverseMass(const real im)
{
	inverseMass = im;
}

real Particle::getMass() const
{
	return inverseMass == 0 ? REAL_MAX : (((real)1.0) / inverseMass);
}

real Particle::getInverseMass() const
{
	return inverseMass;
}

void Particle::integrate(real duration)
{
	//Update position
	position.addScaledVector(velocity, duration);
	//Update acceleration
	Vector3 newAcc = acceleration;
	newAcc.addScaledVector(forceAccum, inverseMass);
	//Update velocity
	velocity.addScaledVector(newAcc, duration);
	//impose drag
	velocity *= real_pow(damping,duration);

	clearAccumulator();

}

void Particle::clearAccumulator()
{
	forceAccum.clear();
}

void Particle::addForce(const Vector3& force)
{
	forceAccum += force;
}

void Particle::getVelocity(Vector3* velocity) const
{
	*velocity = this->velocity;
}

void Particle::setVelocity(const Vector3& v)
{
	velocity = v;
}

void Particle::getPosition(Vector3* position) const
{
	*position = this->position;
}

Vector3 Particle::getPosition() const
{
	return position;
}

void Particle::setPosition(const Vector3& position)
{
	this->position = position;
}

Vector3 Particle::getAcceleration() const
{
	return acceleration;
}

void Particle::setAcceleration(const Vector3& acceleration)
{
	this->acceleration = acceleration;
}

void Particle::setDamping(real damping)
{
	this->damping = damping;
}

real Particle::getDamping() const
{
	return damping;
}

void Particle::setVelocity(real x, real y, real z)
{
	Vector3 v = { x,y,z };
	velocity = v;
}

void Particle::setPosition(real x, real y, real z)
{
	Vector3 p = { x,y,z };
	position = p;
}

void Particle::setAcceleration(real x, real y, real z)
{
	acceleration = { x,y,z };
}