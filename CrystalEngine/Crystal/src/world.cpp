#include<crystal/world.h>

using namespace crystal;

World::World(unsigned maxContacts, unsigned iterations):
	resolver(maxContacts*iterations),
	firstContactGen(NULL),
	maxContacts(maxContacts), bodyCount(0),activeBodyCount(0),
	colliders(),collectGap(DEFAULT_COLLECT_GAP),collisionCallbacks(0),indexList(0)
{
	contacts = new Contact[maxContacts];
	calculateIterations = (iterations == 0);
	cData.contactArray = contacts;
}

World::~World() 
{
	delete[] contacts;
}

void World::addCallbackMethod(RigidBody* body, CallbackMethod(method))
{
	CollisionPrimitive* collider = getAttachedCollider(body);
	unsigned currentIndex = collisionCallbacks.size();
	collisionCallbacks.push_back(method);
	ColliderCallbackIndex index;
	index.collider = collider;
	index.index = currentIndex;
	indexList.push_back(index);
}

void World::startFrame()
{
	for (auto body: bodyList)
	{
		body->clearAccumulators();
		body->calculateDerivedData();
	}

	for (auto collider : colliders)
	{
		if (collider->isActive)
			collider->calculateInternals();
	}
}

void World::addCollider(CollisionPrimitive* collider)
{
	colliders.emplace_back(collider);
}

void World::addRigidBody(RigidBody* const body,CollisionPrimitive* const collider)
{
	bodyCount++;
	activeBodyCount++;

	bodyList.emplace_back(body);

	if (collider)
	{
		addCollider(collider);
		bodyColliderReg.emplace_back(body->getId(), collider->getId());
	}	
}

void World::runPhysics(real duration)
{
	//Apply all forces
	forceRegistry.updateForces(duration);

	//Integrate bodies
	for (auto body : bodyList)
	{
		body->integrate(duration);
	}

	// Generate contacts
	unsigned usedContacts = generateContacts();

	// And process them
	if (calculateIterations) resolver.setIterations(usedContacts * 4);
	resolver.resolveContacts(cData.contactArray, cData.contactCount, duration);
}

void World::deleteBody(RigidBody* body)
{
	body->isActive = false;
	activeBodyCount--;
	CollisionPrimitive* collider = getAttachedCollider(body);
	if (collider)
	{
		collider->isActive = false;
	}
}

void World::removeInActiveBodies()
{
	//No body to remove
	if (activeBodyCount == bodyCount) return;
	
	//Remove inactive colliders
	for (auto itor = colliders.begin(); itor != colliders.end();)
	{
		if (!(*itor)->isActive)
		{
			itor = colliders.erase(itor);
		}
		else
		{
			itor++;
		}
	}
	
	//Remove inactive rigidbodies
	for (auto itor = bodyList.begin(); itor != bodyList.end();)
	{
		if (!(itor->get()->isActive))
		{
			itor = bodyList.erase(itor);
		}
		else
		{
			itor++;
		}
	}
	
	//We have to reset the body pointer in colliders because the original pointer may have moved
	resetColliderBodies();

	bodyCount = activeBodyCount;	
}

unsigned World::generateContacts()
{
	// Set up the collision data structure
	cData.reset(maxContacts);
	cData.friction = (crystal::real)0.9;
	cData.restitution = (crystal::real)0.2;
	cData.tolerance = (crystal::real)0.1;
	unsigned result = 0;
	//Trigger body deletion. Remove all inactive bodies and colliders
	if (bodyCount - activeBodyCount >= collectGap)
	{
		removeInActiveBodies();
	}

	// Perform collision detection
	// The methods in collider_coarse don't work properly for now, so I check all possible contacts instead
	// TODO: Use workable bounding volumn trees to speed up calculation
	CollisionPrimitive* currentCollider;
	CollisionPrimitive* checkCollider;

	for (int i = 0; i < colliders.size(); i++)
	{	
		currentCollider = colliders[i].get();

		if (!(currentCollider->isActive)) continue;

		for (int j = i+1; j < colliders.size(); j++)
		{
			//Check contacts with the rest colliders
			checkCollider = colliders[j].get();

			if (!(checkCollider->isActive)) continue;
			unsigned genCountactNum = CollisionDetector::primitiveCollide(*currentCollider, *checkCollider, &cData);
			if (genCountactNum > 0)
			{
				//Call on collision methods
				for (int i = 0; i < indexList.size(); i++)
				{
					if (indexList[i].collider == currentCollider)
					{
						collisionCallbacks[indexList[i].index](this, currentCollider, checkCollider);
					}
					else if (indexList[i].collider == checkCollider)
					{
						collisionCallbacks[indexList[i].index](this, checkCollider, currentCollider);
					}
				}
			}
			result += genCountactNum;
		}
	}

	return result;
}

CollisionPrimitive* World::getAttachedCollider(RigidBody* body)
{
	unsigned cid = -1;
	for (auto reg : bodyColliderReg)
	{
		if (reg.bodyId == body->getId())
		{
			cid = reg.colliderId;
		}
	}
	if (cid == -1)
		return nullptr;

	for (auto collider : colliders)
	{
		if (collider->getId() == cid)
		{
			return collider.get();
		}
	}
	return nullptr;
}

void World::resetColliderBodies()
{
	for (auto c : colliders)
	{
		//Get body id
		unsigned bid = -1;
		for (auto reg : bodyColliderReg)
		{
			if (reg.colliderId == c->getId())
			{
				bid = reg.bodyId;
				break;
			}				
		}
		//find body
		for (auto b : bodyList)
		{
			if (b->getId() == bid)
			{
				c->body = b.get();
				break;
			}
		}
	}
}