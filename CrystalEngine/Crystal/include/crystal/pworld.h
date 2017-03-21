#pragma once
/*
* Interface file for the particle / mass aggregate world structure.
*/

#include "pfgen.h"
#include "plinks.h"
#include <memory>

//Use lazy deletion for particle effects. 
//When number of destroyable effects is larger than this gap, deletion is triggered
//Deletion is performed in method startFrame()
#ifndef Particle_Collect_Gap
#define Particle_Collect_Gap 10
#endif // !Particle_Collect_Gap


namespace crystal {

	class ParticleEffect;

	using PEffectPtr = std::shared_ptr<ParticleEffect>;
	using ParticlePtr = std::shared_ptr<Particle>;

	/**
	* Keeps track of a set of particles, and provides the means to
	* update them all.
	*/
	class ParticleWorld
	{
	public:
		using Particles = std::vector<ParticlePtr>;
		using ParticleEffects = std::vector<PEffectPtr>;
		using ContactGenerators = std::vector<ParticleContactGenerator*>;

	protected:
		/**
		* Holds the particles
		*/
		Particles particles;

		/*Temp list for deleting destroyable particle effects*/
		ParticleEffects tempList;

		/**
		* True if the world should calculate the number of iterations
		* to give the contact resolver at each frame.
		*/
		bool calculateIterations;

		/**
		* Holds the force generators for the particles in this world.
		*/
		ParticleForceRegistry registry;

		/**
		* Holds the resolver for contacts.
		*/
		ParticleContactResolver resolver;

		/**
		* Contact generators.
		*/
		ContactGenerators contactGenerators;

		/**
		* Holds the list of contacts.
		*/
		ParticleContact *contacts;

		/**
		* Holds the maximum number of contacts allowed (i.e. the
		* size of the contacts array).
		*/
		unsigned maxContacts;

	public:

		/*Holds list of particle effects*/
		ParticleEffects particleEffects;

		/**
		* Creates a new particle simulator that can handle up to the
		* given number of contacts per frame. You can also optionally
		* give a number of contact-resolution iterations to use. If you
		* don't give a number of iterations, then twice the number of
		* contacts will be used.
		*/
		ParticleWorld(unsigned maxContacts = 20, unsigned iterations = 0);

		/**
		* Deletes the simulator.
		*/
		~ParticleWorld();

		/**
		* Calls each of the registered contact generators to report
		* their contacts. Returns the number of generated contacts.
		*/
		unsigned generateContacts();

		/**
		* Integrates all the particles in this world forward in time
		* by the given duration.
		*/
		void integrate(real duration);

		/**
		* Processes all the physics for the particle world.
		*/
		void runPhysics(real duration);

		/**
		* Initializes the world for a simulation frame. This clears
		* the force accumulators for particles in the world. After
		* calling this, the particles can have their forces for this
		* frame added.
		*/
		void startFrame();

		/**
		*  Returns the list of particles.
		*/
		Particles& getParticles();

		/**
		* Returns the list of contact generators.
		*/
		ContactGenerators& getContactGenerators();

		/**
		* Returns the force registry.
		*/
		ParticleForceRegistry& getForceRegistry();

		/*Add a particle to list*/
		void addParticleEffect(ParticleEffect* pe);

	};

	/**
	* A contact generator that takes an STL vector of particle pointers and
	* collides them against the ground.
	*/
	class GroundContacts : public crystal::ParticleContactGenerator
	{
		crystal::ParticleWorld::Particles *particles;

	public:
		void init(crystal::ParticleWorld::Particles *particles);

		virtual unsigned addContact(crystal::ParticleContact *contact,
			unsigned limit) const;
	};

	/* Base class for particle effects. Extend this class to implement Particle effects */
	class ParticleEffect
	{
		friend class ParticleWorld;

	protected:
		
		ParticleWorld::Particles particles;
		Vector3 position;
		bool isPlaying;
		ParticleWorld* pworld;
		bool destroyable;//The particle effect can be destroyed

	public:

		/* Init/Reinit the particle effect. After initing the effect is read to play*/
		virtual void init(crystal::Vector3 position) {};
		/* Play the particle effects */
		virtual void play() {};
		virtual void drawEffect(float deltaTime) {};
		/* Stop playing effects */
		virtual void stop() {};
		virtual ~ParticleEffect() {};
	};

} // namespace crystal

