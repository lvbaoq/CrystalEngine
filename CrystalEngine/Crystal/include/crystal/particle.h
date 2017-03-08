#pragma once
#include "core.h"
namespace crystal {
	/**
	 * Represent the simplest object that can be simulated in the physics system
	 */
	class Particle
	{
	public:
	
		Particle();

		void setDamping(real damping);

		real getDamping() const;

		void setInverseMass(real im);

		real getInverseMass() const;

		/* Set the mass of the particle. Mass can not be zero */
		void setMass(const real mass);

		real getMass() const;


		/**
		* Integrates the particle forward in time by the given amount.
		* This function uses a Newton-Euler integration method, which is a
		* linear approximation to the correct integral. For this reason it
		* may be inaccurate in some cases.
		*/
		void integrate(real duration);


		/**
		* Clears the forces applied to the particle. This will be
		* called automatically after each integration step.
		*/
		void clearAccumulator();

		/* Add a force to the particle, to be applied at the next iteration only.  */
		void addForce(const Vector3& force);

		void getVelocity(Vector3* velocity) const;

		Vector3 getVelocity() const { return velocity; }

		void getPosition(Vector3* position) const;

		Vector3 getPosition() const;

		void setVelocity(const Vector3& velocity);

		void setVelocity(real x, real y, real z);

		void setPosition(const Vector3& position);

		void setPosition(real x, real y, real z);

		Vector3 getAcceleration() const;

		void setAcceleration(real x, real y, real z);

		void setAcceleration(const Vector3& acceleration);

	protected:
		/* The inverse of the mass of the particle
		 * Use inverse of mass for simpler acceleration calculation( since: a = 1/m * f)
		 */
		real inverseMass;

		/** Holds the amount of damping applied to linear motion.
		* Damping is required to remove energy added through
		* numerical instability in the integrator.
		* If the damping is zero, then the velocity will be reduced to nothing;
		* A value of 1 means that the object keeps all its velocity
		* If you don't want the object to look like it is experiencing drag,
		* then values near but less than 1 are optimal¡ª0.995, for example.
		*/
		real damping;

		/**
		* Holds the accumulated force to be applied at the next
		* simulation iteration only. This value is zeroed at each
		* integration step.
		*/
		Vector3 forceAccum;

		/* The linear velocity of the particle in world space */
		Vector3 velocity;

		/* The linear position of the particle in world space */
		Vector3 position;

		/* The acceleration of the particle */
		Vector3 acceleration;

	};
}