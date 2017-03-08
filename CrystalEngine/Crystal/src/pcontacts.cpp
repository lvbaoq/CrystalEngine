#include <crystal/pcontacts.h>

using namespace crystal;

void ParticleContact::resolve(real duration)
{
	resolveVelocity(duration);
	resolveInterpenetration(duration);
}

real ParticleContact::calculateSeparatingVelocity() const
{
	//In particle[0]'s perspective
	Vector3 relativeVelocity = particle[0]->getVelocity();
	if (particle[1]) relativeVelocity -= particle[1]->getVelocity();
	return relativeVelocity * contactNormal;
}

void ParticleContact::resolveVelocity(real duration) const
{
	real separatingVelocity = calculateSeparatingVelocity();
	if (separatingVelocity > 0) return;//separating, no impulse needed

	real newSepVelocity = -restitution * separatingVelocity;

	// Check the velocity build-up due to acceleration only. 
	Vector3 accCausedVelocity = particle[0]->getAcceleration();
	if (particle[1]) accCausedVelocity -= particle[1]->getAcceleration();
	real accCausedSepVelocity = accCausedVelocity * contactNormal * duration;

	if (accCausedSepVelocity < 0)
	{
		newSepVelocity += accCausedSepVelocity * restitution;
		if (newSepVelocity < 0)
		{
			newSepVelocity = 0;
		}
	}

	real deltaVelocity = newSepVelocity - separatingVelocity;//Change of relative velocity
	real totalInverseMass = particle[0]->getInverseMass();
	if (particle[1]) totalInverseMass += particle[1]->getInverseMass();
	//both have infinite mass
	if (totalInverseMass <= 0) return;
	//Calculate impulse
	real impulse = deltaVelocity / totalInverseMass;
	//apply to cotact direction
	Vector3 impulsePerIMass = contactNormal * impulse;
	//change each velocity based on mass proportion
	particle[0]->setVelocity(particle[0]->getVelocity() + 
		impulsePerIMass * particle[0]->getInverseMass());

	if (particle[1])
	{
		//particle[1] goes in the oppsite direction
		particle[1]->setVelocity(particle[1]->getVelocity() +
			impulsePerIMass * -particle[1]->getInverseMass());
	}
}

void ParticleContact::resolveInterpenetration(real duration)
{
	//no interpenetration
	if (penetration <= 0) return;
	//move each particle based on mass
	real totalInverseMass = particle[0]->getInverseMass();
	if (particle[1]) totalInverseMass += particle[1]->getInverseMass();
	//both have infinite mass
	if (totalInverseMass <= 0) return;

	Vector3 movementPerIMass = contactNormal * (penetration / totalInverseMass);

	particle[0]->setPosition(particle[0]->getPosition() + 
		movementPerIMass * particle[0]->getInverseMass());
	if (particle[1])
	{
		particle[1]->setPosition(particle[1]->getPosition() +
			movementPerIMass * - particle[1]->getInverseMass());
	}

	// Calculate the the movement amounts
	particleMovement[0] = movementPerIMass * particle[0]->getInverseMass();
	if (particle[1]) {
		particleMovement[1] = movementPerIMass * -particle[1]->getInverseMass();
	}
	else 
	{
		particleMovement[1].clear();
	}

	// Apply the penetration resolution
	particle[0]->setPosition(particle[0]->getPosition() + particleMovement[0]);
	if (particle[1]) {
		particle[1]->setPosition(particle[1]->getPosition() + particleMovement[1]);
	}
}

ParticleContactResolver::ParticleContactResolver(unsigned iterations):iterations(iterations){}

void ParticleContactResolver::setIterations(unsigned iterations)
{
	ParticleContactResolver::iterations = iterations;
}

void ParticleContactResolver::resolveContacts(ParticleContact *contactArray,unsigned numContacts,real duration)
{
	unsigned i;

	iterationsUsed = 0;

	while (iterationsUsed < iterations)
	{
		// Find the contact with the largest closing velocity;
		real max = REAL_MAX;
		unsigned maxIndex = numContacts;
		for (i = 0; i < numContacts; i++)
		{
			real sepVel = contactArray[i].calculateSeparatingVelocity();
			if (sepVel < max &&
				(sepVel < 0 || contactArray[i].penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		// Do we have anything worth resolving?
		if (maxIndex == numContacts) break;

		// Resolve this contact
		contactArray[maxIndex].resolve(duration);

		// Update the interpenetrations for all particles
		Vector3 *move = contactArray[maxIndex].particleMovement;

		for (i = 0; i < numContacts; i++)
		{
			if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0])
			{
				contactArray[i].penetration -= move[0] * contactArray[i].contactNormal;
			}
			else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1])
			{
				contactArray[i].penetration -= move[1] * contactArray[i].contactNormal;
			}

			if (contactArray[i].particle[1])
			{
				if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0])
				{
					contactArray[i].penetration += move[0] * contactArray[i].contactNormal;
				}
				else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1])
				{
					contactArray[i].penetration += move[1] * contactArray[i].contactNormal;
				}
			}
		}

		iterationsUsed++;
	}
}