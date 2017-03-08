#pragma once
#include "particle.h"
#include <vector>

namespace crystal { 

	/**
	* Common interface for force generators
	* A force generator is asked to add a force to one or more particles
	*/
	class ParticleForceGenerator
	{
	public:
		/**
		* param duration: the duration of the frame for which the force is needed
		*/
		virtual void updateForce(Particle* particle, real duration) = 0;
	};

	/** * Holds all the force generators and the particles they apply to. */
	class ParticleForceRegistry
	{ 
	protected:
	/* Keeps track of one force generator and the particle it applies to. */ 
		struct ParticleForceRegistration 
		{ 
			Particle *particle; 
			ParticleForceGenerator *fg; 
		};
	/* Holds the list of registrations. */ 
		typedef std::vector<ParticleForceRegistration> Registry; 
		Registry registrations;

	public: 
		/* Registers the given force generator to apply to the * given particle. */ 
		void add(Particle* particle, ParticleForceGenerator *fg);
		/** 
		  * Removes the given registered pair from the registry. 
		  * If the pair is not registered, this method will have no effect. 
		  */
		void remove(Particle* particle, ParticleForceGenerator *fg);
		/** 
		  * Clears all registrations from the registry. This will 
		  * not delete the particles or the force generators 
		  * themselves, just the records of their connection. */ 
		void clear();
		/* Calls all the force generators to update the forces of their corresponding particles. */
		void updateForces(real duration);
};

	/**
	 * Simulate gravity force
	 */
	class ParticleGravity : public ParticleForceGenerator
	{
	private:
		/* Holds the accelaration due to gravity */
		Vector3 g;
	public:
		ParticleGravity(const Vector3& gravity);
		virtual void updateForce(Particle* particle, real duration);
	};

	/**
	 * Simulate drag force
	 * Using formula k1 * |v| + k2 * |v| * |v| to calculate magnitude of drag force
	 * where |v| is the speed, and k1 , k2 are called drag coefficients
	 * The direction of drag force is the oppsite of velocity
	 */
	class ParticleDrag:public ParticleForceGenerator
	{
	private:
		real k1, k2;
	public:
		ParticleDrag(real k1, real k2) :k1(k1), k2(k2) {}
		virtual void updateForce(Particle* particle, real duration);
	};

	/**
	 * Simulate spring force
	 * The spring is attached to two objects on both ends
	 */
	class ParticleSpring :public ParticleForceGenerator
	{
	private:
		/* Holds the particle on the other end */
		Particle* other;
		real springConstant;
		real restLength;
	public:
		ParticleSpring(Particle* other, real springConstant, real restLength) :
			other(other), springConstant(springConstant), restLength(restLength) {}
		virtual void updateForce(Particle* particle, real duration);
	};

	/**
	 * Simulate spring force with a fixed anchor
	 */
	class ParticleAnchoredSpring :public ParticleForceGenerator
	{
	private:
		Vector3* anchor;
		real springConst;
		real restLength;
	public:
		ParticleAnchoredSpring(Vector3* anchor, real springConst, real restLength) :
			anchor(anchor), springConst(springConst), restLength(restLength) {}
		virtual void updateForce(Particle* particle, real duration);
		void setAnchor(Vector3* anchor);
	};

	/**
	* A force generator that applies a spring force only
	* when extended.
	*/
	class ParticleBungee : public ParticleForceGenerator
	{
		/** The particle at the other end of the spring. */
		Particle *other;
		/** Holds the sprint constant. */
		real springConstant;
		/*Holds the length of the bungee at the point it begins togenerator a force.*/
		real restLength;
	public:
		/** Creates a new bungee with the given parameters. */
		ParticleBungee(Particle *other, real springConstant, real restLength) :
			other(other), springConstant(springConstant), restLength(restLength) {}
		/** Applies the spring force to the given particle. */
		virtual void updateForce(Particle *particle, real duration);
	};

	/**

	* A force generator that applies a buoyancy force for a plane of

	* liquid parrallel to XZ plane.

	*/

	class ParticleBuoyancy : public ParticleForceGenerator

	{
		/**
		* The maximum submersion depth of the object before
		* it generates its maximum boyancy force.
		*/
		real maxDepth;

		/**
		* The volume of the object.
		*/
		real volume;

		/**
		* The height of the water plane above y=0. The plane will be
		* parrallel to the XZ plane.
		*/
		real waterHeight;

		/**
		* The density of the liquid. Pure water has a density of
		* 1000kg per cubic meter.
		*/
		real liquidDensity;

	public:
		/** Creates a new buoyancy force with the given parameters. */
		ParticleBuoyancy(real maxDepth, real volume, real waterHeight,
			real liquidDensity = 1000.0f) :maxDepth(maxDepth), volume(volume), waterHeight(waterHeight), liquidDensity(liquidDensity) {}

		/** Applies the buoyancy force to the given particle. */
		virtual void updateForce(Particle *particle, real duration);
	};
}