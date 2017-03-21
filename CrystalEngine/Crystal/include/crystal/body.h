#pragma once
#include "core.h"

namespace crystal {
	class RigidBody 
	{
	protected:

		//holds inverse mass of the rigidbody
		real inverseMass;

		//holds mass of the rigidbody
		real mass;
		
		//holds the linear position in world space
		Vector3 position;

		//holds the linear velocity in world space
		Vector3 velocity;

		/**
		* Holds the acceleration of the rigid body.  This value
		* can be used to set acceleration due to gravity (its primary
		* use), or any other constant acceleration.
		*/
		Vector3 acceleration;

		//holds the angular orientation in world space
		Quaternion orientation;

		//holds the angular velocity, or rotation of the rigid body in world space 
		Vector3 rotation;
		/** 
		 * Holds the amount of damping applied to angular 
		 * motion. Damping is required to remove energy added 
		 * through numerical instability in the integrator. 
		 */
		real angularDamping;
		/**
		* Holds the amount of damping applied to linear
		* motion. Damping is required to remove energy added
		* through numerical instability in the integrator.
		*/
		real linearDamping;
		/**
		* Holds the accumulated force to be applied at the next
		* integration step.
		*/
		Vector3 forceAccum;

		/**
		* Holds the accumulated torque to be applied at the next
		* integration step.
		*/
		Vector3 torqueAccum;

		//holds the transform matrix for converting between local space and world space
		Matrix4 transformMatrix;

		/** 
		 * Holds the inverse of the body¡¯s inertia tensor. The inertia 
		 * tensor provided must not be degenerate (that would mean 
		 * the body had zero inertia for spinning along one axis). 
		 * As long as the tensor is finite, it will be invertible. 
		 * The inverse tensor is used for similar reasons as those 
		 * for the use of inverse mass. 
		 * 
		 * The inertia tensor, unlike the other variables that define *
		 a rigid body, is given in body space. 
		 * 
		 * @see inverseMass 
		 */
		Matrix3 inverseInertiaTensor;

		/**
		* Holds the inverse inertia tensor of the body in world
		* space. The inverse inertia tensor member is specified in
		* the body's local space.
		*
		* @see inverseInertiaTensor
		*/
		Matrix3 inverseInertiaTensorWorld;

		/**
		* Holds the linear acceleration of the rigid body, for the
		* previous frame.
		*/
		Vector3 lastFrameAcceleration;

		/**
		* A body can be put to sleep to avoid it being updated
		* by the integration functions or affected by collisions
		* with the world.
		*/
		bool isAwake;

		/**
		* Some bodies may never be allowed to fall asleep.
		* User controlled bodies, for example, should be
		* always awake.
		*/
		bool canSleep;

		/**
		* Holds the amount of motion of the body. This is a recency
		* weighted mean that can be used to put a body to sleep.
		*/
		real motion;

		int id;//Id is used to uniquely mark rigidbodies

	private:
		static unsigned CurrentID;

	public:

		unsigned getId()
		{
			return id;
		}

		RigidBody() :tag(""), canSleep(true), linearFactor(1.0f), angularFactor(1.0f), isActive(true) 
		{ id = RigidBody::CurrentID++; }

		/* A tag string attached to the rigidbody */
		String tag;
		/*If a rigidbody is not active, it can not be seen in the scene*/
		bool isActive;

		/**  A factor controls how fast the body moves.
		* The linear acceleration is multiplied by this value before velocity calculation
		*/
		real linearFactor;
		real angularFactor;//Affects angular accelerations

		virtual bool isPrimitive() { return false; };
		/** 
		 * Calculates internal data from state data. This should be called 
		 * after the body¡¯s state is altered directly (it is called 
		 * automatically during integration). If you change the body¡¯s 
		 * state and then intend to integrate before querying any data 
		 * (such as the transform matrix), then you can omit this step. 
		 */ 
		void calculateDerivedData();

		/** 
		 * Add a force to the center of the rigid body 
		 * The force is expressed in WORLD coordinates
		 */
		void addForce(const Vector3& force);

		void integrate(real duration);

		void clearAccumulators()
		{
			forceAccum.clear();
			torqueAccum.clear();
		}

		/**
		* Adds the given force to the given point on the rigid body.
		* Both the force and the
		* application point are given in world space. Because the
		* force is not applied at the centre of mass, it may be split
		* into both a force and torque.
		*
		* @param force The force to apply.
		*
		* @param point The location at which to apply the force, in
		* world-coordinates.
		*/
		void addForceAtPoint(const Vector3 &force, const Vector3 &point);

		/**
		* Adds the given force to the given point on the rigid body.
		* The direction of the force is given in world coordinates,
		* but the application point is given in body space. This is
		* useful for spring forces, or other forces fixed to the
		* body.
		*
		* @param force The force to apply.
		*
		* @param point The location at which to apply the force, in
		* body-coordinates.
		*/

		void addForceAtBodyPoint(const Vector3 &force, const Vector3 &point);

		/**
		* Adds the given torque to the rigid body.
		* The force is expressed in world-coordinates.
		*
		* @param torque The torque to apply.
		*/
		void addTorque(const Vector3 &torque);


		/**
		* Converts the given point from world space into the body's
		* local space.
		*
		* @param point The point to covert, given in world space.
		*
		* @return The converted point, in local space.
		*/
		Vector3 getPointInLocalSpace(const Vector3 &point) const;

		/**
		* Converts the given point from world space into the body's
		* local space.
		*
		* @param point The point to covert, given in local space.
		*
		* @return The converted point, in world space.
		*/
		Vector3 getPointInWorldSpace(const Vector3 &point) const;

		/**
		* Converts the given direction from world space into the
		* body's local space.
		*
		* @note When a direction is converted between frames of
		* reference, there is no translation required.
		*
		* @param direction The direction to covert, given in world
		* space.
		*
		* @return The converted direction, in local space.
		*/
		Vector3 getDirectionInLocalSpace(const Vector3 &direction) const;

		real getMass() const
		{
			return mass;
		}

		void setMass(real mass);

		real getInverseMass() const
		{
			return inverseMass;
		}

		bool hasFiniteMass() const
		{
			return inverseMass > 0.0f;
		}

		void setAcceleration(Vector3 acc)
		{
			this->acceleration = acc;
		}

		void setAcceleration(real x, real y, real z)
		{
			this->acceleration.x = x;
			this->acceleration.y = y;
			this->acceleration.z = z;
		}

		void setInverseMass(real mass)
		{
			inverseMass = mass;
		}

		/**
		* Sets the intertia tensor for the rigid body.
		*
		* @param inertiaTensor The inertia tensor for the rigid
		* body. This must be a full rank matrix and must be
		* invertible.
		*
		* @warning This invalidates internal data for the rigid body.
		* Either an integration function, or the calculateInternals
		* function should be called before trying to get any settings
		* from the rigid body.
		*/
		void setInertiaTensor(const Matrix3 &inertiaTensor);

		/**
		* Copies the current inertia tensor of the rigid body into
		* the given matrix.
		*
		* @param inertiaTensor A pointer to a matrix to hold the
		* current inertia tensor of the rigid body. The inertia
		* tensor is expressed in the rigid body's local space.
		*/
		void getInertiaTensor(Matrix3 *inertiaTensor) const;

		/**
		* Gets a copy of the current inertia tensor of the rigid body.
		*
		* @return A new matrix containing the current intertia
		* tensor. The inertia tensor is expressed in the rigid body's
		* local space.
		*/
		Matrix3 getInertiaTensor() const;

		/**
		* Copies the current inertia tensor of the rigid body into
		* the given matrix.
		*
		* @param inertiaTensor A pointer to a matrix to hold the
		* current inertia tensor of the rigid body. The inertia
		* tensor is expressed in world space.
		*/
		void getInertiaTensorWorld(Matrix3 *inertiaTensor) const;

		/**
		* Gets a copy of the current inertia tensor of the rigid body.
		*
		* @return A new matrix containing the current intertia
		* tensor. The inertia tensor is expressed in world space.
		*/
		Matrix3 getInertiaTensorWorld() const;

		/**
		* Sets the inverse intertia tensor for the rigid body.
		*
		* @param inverseInertiaTensor The inverse inertia tensor for
		* the rigid body. This must be a full rank matrix and must be
		* invertible.
		*
		* @warning This invalidates internal data for the rigid body.
		* Either an integration function, or the calculateInternals
		* function should be called before trying to get any settings
		* from the rigid body.
		*/
		void setInverseInertiaTensor(const Matrix3 &inverseInertiaTensor);

		/**
		* Copies the current inverse inertia tensor of the rigid body
		* into the given matrix.
		*
		* @param inverseInertiaTensor A pointer to a matrix to hold
		* the current inverse inertia tensor of the rigid body. The
		* inertia tensor is expressed in the rigid body's local
		* space.
		*/
		void getInverseInertiaTensor(Matrix3 *inverseInertiaTensor) const;

		/**
		* Gets a copy of the current inverse inertia tensor of the
		* rigid body.
		*
		* @return A new matrix containing the current inverse
		* intertia tensor. The inertia tensor is expressed in the
		* rigid body's local space.
		*/
		Matrix3 getInverseInertiaTensor() const;

		/**
		* Copies the current inverse inertia tensor of the rigid body
		* into the given matrix.
		*
		* @param inverseInertiaTensor A pointer to a matrix to hold
		* the current inverse inertia tensor of the rigid body. The
		* inertia tensor is expressed in world space.
		*/
		void getInverseInertiaTensorWorld(Matrix3 *inverseInertiaTensor) const;

		/**
		* Gets a copy of the current inverse inertia tensor of the
		* rigid body.
		*
		* @return A new matrix containing the current inverse
		* intertia tensor. The inertia tensor is expressed in world
		* space.
		*/
		Matrix3 getInverseInertiaTensorWorld() const;

		/**
		* Sets both linear and angular damping in one function call.
		*
		* @param linearDamping The speed that velocity is shed from
		* the rigid body.
		*
		* @param angularDamping The speed that rotation is shed from
		* the rigid body.
		*
		* @see setLinearDamping
		* @see setAngularDamping
		*/
		void setDamping(const real linearDamping, const real angularDamping);

		/**
		* Sets the linear damping for the rigid body.
		*
		* @param linearDamping The speed that velocity is shed from
		* the rigid body.
		*
		* @see setAngularDamping
		*/
		void setLinearDamping(const real linearDamping);

		/**
		* Gets the current linear damping value.
		*
		* @return The current linear damping value.
		*/
		real getLinearDamping() const;

		/**
		* Sets the angular damping for the rigid body.
		*
		* @param angularDamping The speed that rotation is shed from
		* the rigid body.
		*
		* @see setLinearDamping
		*/
		void setAngularDamping(const real angularDamping);
		
		/**
		* Gets the current angular damping value.
		*
		* @return The current angular damping value.
		*/
		real getAngularDamping() const;

		/**
		* Fills the given vector with the current accumulated value
		* for linear acceleration. The acceleration accumulators
		* are set during the integration step. They can be read to
		* determine the rigid body's acceleration over the last
		* integration step. The linear acceleration is given in world
		* space.
		*
		* @param linearAcceleration A pointer to a vector to receive
		* the linear acceleration data.
		*/
		void getLastFrameAcceleration(Vector3 *linearAcceleration) const;

		/**
		* Gets the current accumulated value for linear
		* acceleration. The acceleration accumulators are set during
		* the integration step. They can be read to determine the
		* rigid body's acceleration over the last integration
		* step. The linear acceleration is given in world space.
		*
		* @return The rigid body's linear acceleration.
		*/
		Vector3 getLastFrameAcceleration() const;

		/**
		* Sets the position of the rigid body.
		*
		* @param position The new position of the rigid body.
		*/
		void setPosition(const Vector3 &position);

		/**
		* Sets the position of the rigid body by component.
		*
		* @param x The x coordinate of the new position of the rigid
		* body.
		*
		* @param y The y coordinate of the new position of the rigid
		* body.
		*
		* @param z The z coordinate of the new position of the rigid
		* body.
		*/
		void setPosition(const real x, const real y, const real z);

		/**
		* Fills the given vector with the position of the rigid body.
		*
		* @param position A pointer to a vector into which to write
		* the position.
		*/
		void getPosition(Vector3 *position) const;

		/**
		* Gets the position of the rigid body.
		*
		* @return The position of the rigid body.
		*/
		Vector3 getPosition() const;

		/**
		* Sets the orientation of the rigid body.
		*
		* @param orientation The new orientation of the rigid body.
		*
		* @note The given orientation does not need to be normalised,
		* and can be zero. This function automatically constructs a
		* valid rotation quaternion with (0,0,0,0) mapping to
		* (1,0,0,0).
		*/
		void setOrientation(const Quaternion &orientation);

		/**
		* Sets the orientation of the rigid body by component.
		*
		* @param r The real component of the rigid body's orientation
		* quaternion.
		*
		* @param i The first complex component of the rigid body's
		* orientation quaternion.
		*
		* @param j The second complex component of the rigid body's
		* orientation quaternion.
		*
		* @param k The third complex component of the rigid body's
		* orientation quaternion.
		*
		* @note The given orientation does not need to be normalised,
		* and can be zero. This function automatically constructs a
		* valid rotation quaternion with (0,0,0,0) mapping to
		* (1,0,0,0).
		*/
		void setOrientation(const real r, const real i,
			const real j, const real k);
		/**
		* Fills the given quaternion with the current value of the
		* rigid body's orientation.
		*
		* @param orientation A pointer to a quaternion to receive the
		* orientation data.
		*/
		void getOrientation(Quaternion *orientation) const;

		/**
		* Gets the orientation of the rigid body.
		* @return The orientation of the rigid body.
		*/
		Quaternion getOrientation() const;

		/**
		* Fills the given matrix with a transformation representing
		* the rigid body's orientation.
		*
		* @note Transforming a direction vector by this matrix turns
		* it from the body's local space to world space.
		*
		* @param matrix A pointer to the matrix to fill.
		*/
		void getOrientation(Matrix3 *matrix) const;

		/**
		* Fills the given matrix data structure with a transformation
		* representing the rigid body's orientation.
		*
		* @note Transforming a direction vector by this matrix turns
		* it from the body's local space to world space.
		*
		* @param matrix A pointer to the matrix to fill.
		*/
		void getOrientation(real matrix[9]) const;

		/**
		* Fills the given matrix with a transformation representing
		* the rigid body's position and orientation.
		*
		* @note Transforming a vector by this matrix turns it from
		* the body's local space to world space.
		*
		* @param transform A pointer to the matrix to fill.
		*/
		void getTransform(Matrix4 *transform) const;

		/**
		* Fills the given matrix data structure with a
		* transformation representing the rigid body's position and
		* orientation.
		*
		* @note Transforming a vector by this matrix turns it from
		* the body's local space to world space.
		*
		* @param matrix A pointer to the matrix to fill.
		*/
		void getTransform(real matrix[16]) const;

		/**
		* Fills the given matrix data structure with a
		* transformation representing the rigid body's position and
		* orientation. The matrix is transposed from that returned
		* by getTransform. This call returns a matrix suitable
		* for applying as an OpenGL transform.
		*
		* @note Transforming a vector by this matrix turns it from
		* the body's local space to world space.
		*
		* @param matrix A pointer to the matrix to fill.
		*/
		void getGLTransform(float matrix[16]) const;

		/**
		* Gets a transformation representing the rigid body's
		* position and orientation.
		*
		* @note Transforming a vector by this matrix turns it from
		* the body's local space to world space.
		*
		* @return The transform matrix for the rigid body.
		*/
		Matrix4 getTransform() const 
		{
			return transformMatrix;
		}
		/**
		* Sets the velocity of the rigid body.
		*
		* @param velocity The new velocity of the rigid body. The
		* velocity is given in world space.
		*/
		void setVelocity(const Vector3 &velocity);

		/**
		* Sets the velocity of the rigid body by component. The
		* velocity is given in world space.
		*
		* @param x The x coordinate of the new velocity of the rigid
		* body.
		*
		* @param y The y coordinate of the new velocity of the rigid
		* body.
		*
		* @param z The z coordinate of the new velocity of the rigid
		* body.
		*/
		void setVelocity(const real x, const real y, const real z);

		/**
		* Fills the given vector with the velocity of the rigid body.
		*
		* @param velocity A pointer to a vector into which to write
		* the velocity. The velocity is given in world local space.
		*/
		void getVelocity(Vector3 *velocity) const;

		/**
		* Gets the velocity of the rigid body.
		*
		* @return The velocity of the rigid body. The velocity is
		* given in world local space.
		*/
		Vector3 getVelocity() const;

		/**
		* Applies the given change in velocity.
		*/
		void addVelocity(const Vector3 &deltaVelocity);

		/**
		* Sets the rotation of the rigid body.
		*
		* @param rotation The new rotation of the rigid body. The
		* rotation is given in world space.
		*/
		void setRotation(const Vector3 &rotation);

		/**
		* Sets the rotation of the rigid body by component. The
		* rotation is given in world space.
		*
		* @param x The x coordinate of the new rotation of the rigid
		* body.
		*
		* @param y The y coordinate of the new rotation of the rigid
		* body.
		*
		* @param z The z coordinate of the new rotation of the rigid
		* body.
		*/
		void setRotation(const real x, const real y, const real z);

		/**
		* Fills the given vector with the rotation of the rigid body.
		*
		* @param rotation A pointer to a vector into which to write
		* the rotation. The rotation is given in world local space.
		*/
		void getRotation(Vector3 *rotation) const;

		/**
		* Gets the rotation of the rigid body.
		*
		* @return The rotation of the rigid body. The rotation is
		* given in world local space.
		*/
		Vector3 getRotation() const;

		/**
		* Applies the given change in rotation.
		*/
		void addRotation(const Vector3 &deltaRotation);

		/**
		* Returns true if the body is awake and responding to
		* integration.
		*
		* @return The awake state of the body.
		*/
		bool getAwake() const
		{
			return isAwake;
		}

		/**
		* Sets the awake state of the body. If the body is set to be
		* not awake, then its velocities are also cancelled, since
		* a moving body that is not awake can cause problems in the
		* simulation.
		*
		* @param awake The new awake state of the body.
		*/
		void setAwake(const bool awake = true);

		/**
		* Returns true if the body is allowed to go to sleep at
		* any time.
		*/
		bool getCanSleep() const
		{
			return canSleep;
		}

		/**
		* Sets whether the body is ever allowed to go to sleep. Bodies
		* under the player's control, or for which the set of
		* transient forces applied each frame are not predictable,
		* should be kept awake.
		*
		* @param canSleep Whether the body can now be put to sleep.
		*/
		void setCanSleep(const bool canSleep = true);
	};
}