#pragma once
#include "body.h"
#include "contact.h"
#include "fgen.h"
#include "collide_fine.h"
#include <memory>

#define DEFAULT_COLLECT_GAP 2

#ifndef CallbackMethods
#define CallbackMethod(name) void(*name)(World* world,CollisionPrimitive* thisBody,CollisionPrimitive* other)
#endif // !CallbackMethods


#ifndef CallbackList
#define CallbackList std::vector<CallbackMethod()>
#endif // !CallbackList



namespace crystal {

	using BodyPtr = std::shared_ptr<RigidBody>;
	using ColliderPtr = std::shared_ptr<CollisionPrimitive>;
	using RigidBodyList = std::vector<BodyPtr>;
	using ColliderList = std::vector<ColliderPtr>;

	class World
	{
	public:
		
		//Holds all the forces to apply
		ForceRegistry forceRegistry;
	private:
 		/**
		 * True if the world should calculate the number of iterations
		 * to give the contact resolver at each frame.
		 */
		bool calculateIterations;

		/**
		* Holds a rigidbody's id and its corresponding colliders
		* Currently it is a one-one relation.
		*/
		struct RigidBodyRegistration
		{
			unsigned bodyId;
			unsigned colliderId;

			RigidBodyRegistration(unsigned bid,unsigned cid):bodyId(bid),colliderId(cid){}
		};

		std::vector<RigidBodyRegistration> bodyColliderReg;

		/*Holds the list of rigidbodies*/
		RigidBodyList bodyList;

		/**
		* Holds the resolver for sets of contacts.
		*/
		ContactResolver resolver;

		/**
		* Holds one contact generators in a linked list.
		*/
		struct ContactGenRegistration
		{
			ContactGenerator *gen;
			ContactGenRegistration *next;
		};

		/* Holds the list of colliders. Only support primitive colliders now */
		ColliderList colliders;

		//Holds number of rigidbodys
		unsigned bodyCount;
		//Holds number of active rigidbodys
		unsigned activeBodyCount;
		//When the difference between colliderCount and activeColliderCount is larger than this gap, 
		//collider list is re-allocated to remove all inactive colliders
		int collectGap;
		/**
		* Holds the head of the list of contact generators.
		*/
		ContactGenRegistration *firstContactGen;

		/**
		* Holds an array of contacts, for filling by the contact
		* generators.
		*/
		Contact *contacts;

		/**
		* Holds the maximum number of contacts allowed (i.e. the size
		* of the contacts array).
		*/
		unsigned maxContacts;

		/** Holds the collision data structure for collision detection. */
		CollisionData cData;
		
		void addCollider(CollisionPrimitive* collider);

		//Holds callback methods
		CallbackList collisionCallbacks;

		/*Holds a collider and its corrensponding callback method's index in @collisionCallbacks*/
		struct ColliderCallbackIndex
		{
			CollisionPrimitive* collider;
			unsigned index;
		};

		std::vector<ColliderCallbackIndex> indexList;

		CollisionPrimitive* getAttachedCollider(RigidBody* body);

		void resetColliderBodies();

	public:
		/**
		* Creates a new simulator that can handle up to the given
		* number of contacts per frame. You can also optionally give
		* a number of contact-resolution iterations to use. If you
		* don't give a number of iterations, then four times the
		* number of detected contacts will be used for each frame.
		*/
		World(unsigned maxContacts = 20, unsigned iterations = 0);
		
		~World();

		void addCallbackMethod(RigidBody* body,CallbackMethod(method));

		RigidBodyList& getRigidBodyList()
		{
			return bodyList;
		}

		void addRigidBody(RigidBody* const body, CollisionPrimitive* const collider = NULL);

		//Delete a rigidbody and its attached collider (if any)
		void deleteBody(RigidBody* body);
		
		//The method deleteBody uses a lazy delete strategy. Call this method to remove all inActive colliders in list
		void removeInActiveBodies();

		/**
		* Calls each of the registered contact generators to report
		* their contacts. Returns the number of generated contacts.
		*/
		unsigned generateContacts();

		/**
		* Processes all the physics for the world.
		*/
		void runPhysics(real duration);

		/**
		* Initialises the world for a simulation frame. This clears
		* the force and torque accumulators for bodies in the
		* world. After calling this, the bodies can have their forces
		* and torques for this frame added.
		*/
		void startFrame();
	};

 }