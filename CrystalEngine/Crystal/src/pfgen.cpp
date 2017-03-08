#include <crystal/pfgen.h>

using namespace crystal;

void ParticleForceRegistry::add(Particle* particle, ParticleForceGenerator* fg)
{
	ParticleForceRegistration pfg;
	pfg.particle = particle;
	pfg.fg = fg;
	registrations.push_back(pfg);

}

void ParticleForceRegistry::remove(Particle* particle, ParticleForceGenerator* fg)
{
	//TODO: Check if the implementation is correct
	Registry::iterator i = registrations.begin();
	for(;i!=registrations.end();i++)
	{
		if (i->particle == particle && i->fg == fg)
		{
			registrations.erase(i);
		}
	}
}

void ParticleForceRegistry::clear()
{
	registrations.clear();
}

void ParticleForceRegistry::updateForces(real duration)
{
	Registry::iterator iterator = registrations.begin();
	for (; iterator != registrations.end(); iterator++)
	{
		iterator->fg->updateForce(iterator->particle, duration);
	}
}

ParticleGravity::ParticleGravity(const Vector3& gravity)
{
	g = gravity;
}

void ParticleGravity::updateForce(Particle* particle, real duration)
{
	particle->addForce(g * particle->getMass());
}

void ParticleDrag::updateForce(Particle* particle, real duration)
{
	Vector3 force;
	particle->getVelocity(&force);

	//Calculate total drag coefficient
	real dragCoeff = force.magnitude();
	dragCoeff = k1 * dragCoeff + k2*dragCoeff*dragCoeff;

	//apply final drag force
	force.normalize();
	force *= -dragCoeff;
	particle->addForce(force);
}

void ParticleSpring::updateForce(Particle* particle, real duration)
{
	//Get vector of spring
	Vector3 force;
	particle->getPosition(&force);
	force -= other->getPosition();

	//Calculate magnitude
	real magnitude = real_abs(force.magnitude() - restLength) * springConstant;
	//Apply final force
	force.normalize();
	force *= -magnitude;
	particle->addForce(force);
}

void ParticleAnchoredSpring::updateForce(Particle* particle, real duration)
{
	//Get vector of spring
	Vector3 force;
	particle->getPosition(&force);
	force -= *anchor;

	//Calculate magnitude
	real magnitude = real_abs(force.magnitude() - restLength) * springConst;
	//Apply final force
	force.normalize();
	force *= -magnitude;
	particle->addForce(force);
}

void ParticleAnchoredSpring::setAnchor(Vector3* anchor)
{
	this->anchor = anchor;
}

void ParticleBungee::updateForce(Particle* particle, real duration)
{
	// Calculate the vector of the spring
	Vector3 force;
	particle->getPosition(&force);
	force -= other->getPosition();

	// Check if the bungee is compressed
	real magnitude = force.magnitude();
	if (magnitude <= restLength) return;

	// Calculate the magnitude of the force
	magnitude = springConstant * (restLength - magnitude);

	// Calculate the final force and apply it
	force.normalize();
	force *= -magnitude;
	particle->addForce(force);
}

void ParticleBuoyancy::updateForce(Particle* particle, real duration)
{
	real posY = particle->getPosition().y;
	//The whole object is above water surface, no buoyancy force is generated
	if ((posY - waterHeight) >= maxDepth) return;

	Vector3 force{ 0,0,0 };

	//The whole object is below water surface, buoyancy force is const
	if (posY <= waterHeight - maxDepth)
	{
		force.y = liquidDensity * volume;
		particle->addForce(force);
		return;
	}

	//Part of the object is below water surface
	// !! different from the book, I think a minus( - ) is needed to make the buoyancy upward
	force.y = - liquidDensity * volume *
		(posY - maxDepth - waterHeight) / (2 * maxDepth);
	particle->addForce(force);
}