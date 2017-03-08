#pragma once
#include "body.h"
#include <vector>

namespace crystal {

	class ForceGenerator
	{
	public:
		/**
		 * Overload this in implementations of the interface to calculate
         * and update the force applied to the given rigid body.
	     */
		virtual void updateForce(RigidBody *body, real duration) = 0;
	};

	/**
	* Holds all the force generators and the bodies they apply to.
	*/
	class ForceRegistry
	{
	protected:

		/**
		* Keeps track of one force generator and the body it
		* applies to.
		*/
		struct ForceRegistration
		{
			RigidBody *body;
			ForceGenerator *fg;
		};

		/**
		* Holds the list of registrations.
		*/
		typedef std::vector<ForceRegistration> Registry;
		Registry registrations;

	public:
		/**
		* Registers the given force generator to apply to the
		* given body.
		*/
		void add(RigidBody* body, ForceGenerator *fg);

		/**
		* Removes the given registered pair from the registry.
		* If the pair is not registered, this method will have
		* no effect.
		*/
		void remove(RigidBody* body, ForceGenerator *fg);

		/**
		* Clears all registrations from the registry. This will
		* not delete the bodies or the force generators
		* themselves, just the records of their connection.
		*/
		void clear();

		/**
		* Calls all the force generators to update the forces of
		* their corresponding bodies.
		*/
		void updateForces(real duration);
	};

	class ConstantForce : public ForceGenerator
	{
	private:
		Vector3 force;
	public:
		ConstantForce(const Vector3& f):force(f){}
		void updateForce(RigidBody *body, real duration);
	};

	class Gravity:public ForceGenerator
	{
	private:
		Vector3 gravity;
	public:
		Gravity(const Vector3& g):gravity(g){}
		virtual void updateForce(RigidBody *body, real duration);
	};

	class Spring :public ForceGenerator
	{
	private:
		real springConstant;
		real restLength;
		RigidBody* other;
		//in local space
		Vector3 connectionPoint;
		//in local space
		Vector3 otherConnectionPoint;

	public:
		Spring(const Vector3 &localConnectionPt,
			RigidBody *other,
			const Vector3 &otherConnectionPt,
			real springConstant,
			real restLength):connectionPoint(localConnectionPt),other(other),
			otherConnectionPoint(otherConnectionPt),springConstant(springConstant),restLength(restLength){}
		virtual void updateForce(RigidBody* body, real duration);
	};

	/**
	* A force generator that applies an aerodynamic force.
	*/
	class Aero : public ForceGenerator
	{
	protected:
		/**
		* Holds the aerodynamic tensor for the surface in body
		* space.
		*/
		Matrix3 tensor;

		/**
		* Holds the relative position of the aerodynamic surface in
		* body coordinates.
		*/
		Vector3 position;

		/**
		* Holds a pointer to a vector containing the windspeed of the
		* environment. This is easier than managing a separate
		* windspeed vector per generator and having to update it
		* manually as the wind changes.
		*/
		const Vector3* windspeed;

	public:
		/**
		* Creates a new aerodynamic force generator with the
		* given properties.
		*/
		Aero(const Matrix3 &tensor, const Vector3 &position,
			const Vector3 *windspeed);

		/**
		* Applies the force to the given rigid body.
		*/
		virtual void updateForce(RigidBody *body, real duration);

	protected:
		/**
		* Uses an explicit tensor matrix to update the force on
		* the given rigid body. This is exactly the same as for updateForce
		* only it takes an explicit tensor.
		*/
		void updateForceFromTensor(RigidBody *body, real duration,
			const Matrix3 &tensor);
	};

	/**
	* A force generator with a control aerodynamic surface. This
	* requires three inertia tensors, for the two extremes and
	* 'resting' position of the control surface.  The latter tensor is
	* the one inherited from the base class, the two extremes are
	* defined in this class.
	*/
	class AeroControl : public Aero
	{
	protected:
		/**
		* The aerodynamic tensor for the surface, when the control is at
		* its maximum value.
		*/
		Matrix3 maxTensor;

		/**
		* The aerodynamic tensor for the surface, when the control is at
		* its minimum value.
		*/
		Matrix3 minTensor;

		/**
		* The current position of the control for this surface. This
		* should range between -1 (in which case the minTensor value
		* is used), through 0 (where the base-class tensor value is
		* used) to +1 (where the maxTensor value is used).
		*/
		real controlSetting;

	private:
		/**
		* Calculates the final aerodynamic tensor for the current
		* control setting.
		*/
		Matrix3 getTensor();

	public:
		/**
		* Creates a new aerodynamic control surface with the given
		* properties.
		*/
		AeroControl(const Matrix3 &base,
			const Matrix3 &min, const Matrix3 &max,
			const Vector3 &position, const Vector3 *windspeed);

		/**
		* Sets the control position of this control. This * should
		range between -1 (in which case the minTensor value is *
		used), through 0 (where the base-class tensor value is used) *
		to +1 (where the maxTensor value is used). Values outside that
		* range give undefined results.
		*/
		void setControl(real value);

		/**
		* Applies the force to the given rigid body.
		*/
		virtual void updateForce(RigidBody *body, real duration);
	};

}