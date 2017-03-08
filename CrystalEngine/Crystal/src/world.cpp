#include<crystal/world.h>

using namespace crystal;

World::World(unsigned maxContacts, unsigned iterations):
	first(NULL),deleteFirst(NULL),
	resolver(maxContacts*iterations),
	firstContactGen(NULL),
	maxContacts(maxContacts), colliderCount(0),activeColliderCount(0),
	colliders(),collectGap(DEFAULT_COLLECT_GAP),collisionCallbacks(0),indexList(0)
{
	contacts = new Contact[maxContacts];
	calculateIterations = (iterations == 0);
	cData.contactArray = contacts;
}

World::~World() 
{
	RigidBodyRegistration* rb = first;
	RigidBodyRegistration* next;
	while (rb->next)
	{
		next = rb->next;
		delete rb;
		rb = next;
	}
	delete rb;

	delete[] contacts;

}

void World::addCallbackMethod(CollisionPrimitive* collider, CallbackMethod(method))
{
	unsigned currentIndex = collisionCallbacks.size();
	collisionCallbacks.push_back(method);
	ColliderCallbackIndex index;
	index.collider = collider;
	index.index = currentIndex;
	indexList.push_back(index);
}

void World::startFrame()
{
	RigidBodyRegistration* br = first;
	while (br)
	{
		br->body->clearAccumulators();
		br->body->calculateDerivedData();
		br = br->next;
	}
	for (int i = 0; i < colliderCount; i++)
	{
		if(colliders[i]->isActive)
			colliders[i]->calculateInternals();
	}
}

void World::addCollider(CollisionPrimitive* collider)
{
	colliderCount++;
	activeColliderCount++;
	colliders.push_back(collider);
}

void World::addRigidBody(RigidBody* const body,CollisionPrimitive* const collider)
{
	RigidBodyRegistration* br = new RigidBodyRegistration();
	br->body = body;
	br->collider = collider;
	br->next = NULL;
	if (first == NULL)
	{
		current = first = br;
	}
	else {
		current->next = br;
		current = br;
	}
	if (collider)
	{
		addCollider(collider);
	}
	
}

void World::runPhysics(real duration)
{
	//Apply all forces
	forceRegistry.updateForces(duration);

	//Integrate bodies
	RigidBodyRegistration* br = first;
	while (br)
	{
		br->body->integrate(duration);
		br = br->next;
	}

	// Generate contacts
	unsigned usedContacts = generateContacts();

	// And process them
	if (calculateIterations) resolver.setIterations(usedContacts * 4);
	resolver.resolveContacts(cData.contactArray, cData.contactCount, duration);
}

void World::deleteBody(RigidBody* body)
{
	RigidBodyRegistration* br = first;
	RigidBodyRegistration* pre = NULL;
	RigidBodyRegistration* next = NULL;
	while (br!= NULL)
	{
		next = br->next;

		if (br->body == body)
		{
			br->next = NULL;
			//Found body, Delete it
			if (pre)
			{
				pre->next = next;
			}
			//delete collider. Use a lazy delete here.
			if (br->collider)
			{
				br->collider->isActive = false;
				activeColliderCount--;
			}
			//Record this body
			if (deleteFirst == NULL)
			{
				deleteCurrent = deleteFirst = br;
			}
			else 
			{
				deleteCurrent->next = br;
				deleteCurrent = br;
			}
		}
		else
		{
			pre = br;
		}

		br = next;		
	}
	if (pre != NULL)
		current = pre;
	else
		current = NULL;
}

void World::removeInActiveColliders()
{
	//No colliders to remove
	if (activeColliderCount == colliderCount) return;
	tempColliderList.clear();
	for (int i = 0; i < colliders.size(); i++)
	{
		if (colliders[i]->isActive)
		{
			tempColliderList.push_back(colliders[i]);
		}
	}
	colliders.clear();
	colliders = tempColliderList;
	colliderCount = activeColliderCount;
	//Clear removed rigidbodies
	RigidBodyRegistration* rb = deleteFirst;
	RigidBodyRegistration* next;
	while (rb->next)
	{
		next = rb->next;
		delete rb;
		rb = next;
	}
	delete rb;
}

unsigned World::generateContacts()
{
	// Set up the collision data structure
	cData.reset(maxContacts);
	cData.friction = (crystal::real)0.9;
	cData.restitution = (crystal::real)0.2;
	cData.tolerance = (crystal::real)0.1;
	unsigned result = 0;
	//Trigger collider deletion. Remove all inactive colliders
	if (colliderCount - activeColliderCount >= collectGap)
	{
		removeInActiveColliders();
	}

	// Perform collision detection
	// The methods in collider_coarse don't work properly for now, so I check all possible contacts instead
	// TODO: Use workable bounding volumn trees to speed up calculation
	CollisionPrimitive* currentCollider;
	CollisionPrimitive* checkCollider;
	for (int i = 0; i < colliderCount; i++)
	{	
		currentCollider = colliders.at(i);

		if (!currentCollider->isActive) continue;

		for (int j = i+1; j < colliderCount; j++)
		{
			//Check contacts with the rest colliders
			checkCollider = colliders.at(j);
			if (!checkCollider->isActive) continue;
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