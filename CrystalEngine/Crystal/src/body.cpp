#include <crystal\body.h>
#include <assert.h>
#include <memory.h>
using namespace crystal;

unsigned RigidBody::CurrentID = 0;

void RigidBody::addForce(const Vector3& force)
{
	forceAccum += force;
}

void RigidBody::addTorque(const Vector3& force)
{
	torqueAccum += force;
}

void RigidBody::integrate(real duration)
{
	//update linear motion
	//update acceleration
	Vector3 lastFrameLinearAcc = acceleration;
	lastFrameLinearAcc.addScaledVector(forceAccum,inverseMass);
	//update velocity
	velocity.addScaledVector(lastFrameLinearAcc*linearFactor, duration);
	//Impose drag
	velocity *= real_pow(linearDamping, duration);
	//update position
	position.addScaledVector(velocity, duration);
	
	//update angular motion
	//Get angular acceleration from torque
	Vector3 angularAcceleration = inverseInertiaTensorWorld.transform(torqueAccum);
	//update rotation speed
	rotation.addScaledVector(angularAcceleration*angularFactor, duration);
	//impose drag
	rotation *= real_pow(angularDamping, duration);
	//update orientation
	orientation.addScaledVector(rotation,duration);

	// Normalise the orientation, and update the matrices with the new position and orientation
	calculateDerivedData();
	//clear both accumlators
	clearAccumulators();
}

void RigidBody::addForceAtPoint(const Vector3& force, const Vector3& point)
{
	//add force
	forceAccum += force;
	//add torque
	Vector3 torque = (point - position) % force;
	torqueAccum += torque;
}

void RigidBody::addForceAtBodyPoint(const Vector3& force, const Vector3& point)
{
	Vector3 wp = getPointInWorldSpace(point);
	addForceAtPoint(force, wp);
}

void RigidBody::setInertiaTensor(const Matrix3& inertiaTensor)
{
	inverseInertiaTensor.setInverse(inertiaTensor);
}

/**
 * Inline function that creates a transform matrix from a
 * position and orientation.
*/
static inline void _calculateTransformMatrix(Matrix4 &transformMatrix,
	const Vector3 &position,
	const Quaternion &orientation)
{
	transformMatrix.data[0] = 1 - 2 * orientation.j*orientation.j -
		2 * orientation.k*orientation.k;
	transformMatrix.data[1] = 2 * orientation.i*orientation.j -
		2 * orientation.r*orientation.k;
	transformMatrix.data[2] = 2 * orientation.i*orientation.k +
		2 * orientation.r*orientation.j;
	transformMatrix.data[3] = position.x;

	transformMatrix.data[4] = 2 * orientation.i*orientation.j +
		2 * orientation.r*orientation.k;
	transformMatrix.data[5] = 1 - 2 * orientation.i*orientation.i -
		2 * orientation.k*orientation.k;
	transformMatrix.data[6] = 2 * orientation.j*orientation.k -
		2 * orientation.r*orientation.i;
	transformMatrix.data[7] = position.y;

	transformMatrix.data[8] = 2 * orientation.i*orientation.k -
		2 * orientation.r*orientation.j;
	transformMatrix.data[9] = 2 * orientation.j*orientation.k +
		2 * orientation.r*orientation.i;
	transformMatrix.data[10] = 1 - 2 * orientation.i*orientation.i -
		2 * orientation.j*orientation.j;
	transformMatrix.data[11] = position.z;
}

/**
* Internal function to do an intertia tensor transform by a quaternion.
* Note that the implementation of this function was created by an
* automated code-generator and optimizer.
*/

static inline void _transformInertiaTensor(Matrix3 &iitWorld,
	const Quaternion &q,
	const Matrix3 &iitBody,
	const Matrix4 &rotmat)
{
	real t4 = rotmat.data[0] * iitBody.data[0] +
		rotmat.data[1] * iitBody.data[3] +
		rotmat.data[2] * iitBody.data[6];
	real t9 = rotmat.data[0] * iitBody.data[1] +
		rotmat.data[1] * iitBody.data[4] +
		rotmat.data[2] * iitBody.data[7];
	real t14 = rotmat.data[0] * iitBody.data[2] +
		rotmat.data[1] * iitBody.data[5] +
		rotmat.data[2] * iitBody.data[8];
	real t28 = rotmat.data[4] * iitBody.data[0] +
		rotmat.data[5] * iitBody.data[3] +
		rotmat.data[6] * iitBody.data[6];
	real t33 = rotmat.data[4] * iitBody.data[1] +
		rotmat.data[5] * iitBody.data[4] +
		rotmat.data[6] * iitBody.data[7];
	real t38 = rotmat.data[4] * iitBody.data[2] +
		rotmat.data[5] * iitBody.data[5] +
		rotmat.data[6] * iitBody.data[8];
	real t52 = rotmat.data[8] * iitBody.data[0] +
		rotmat.data[9] * iitBody.data[3] +
		rotmat.data[10] * iitBody.data[6];
	real t57 = rotmat.data[8] * iitBody.data[1] +
		rotmat.data[9] * iitBody.data[4] +
		rotmat.data[10] * iitBody.data[7];
	real t62 = rotmat.data[8] * iitBody.data[2] +
		rotmat.data[9] * iitBody.data[5] +
		rotmat.data[10] * iitBody.data[8];

	iitWorld.data[0] = t4*rotmat.data[0] +
		t9*rotmat.data[1] +
		t14*rotmat.data[2];
	iitWorld.data[1] = t4*rotmat.data[4] +
		t9*rotmat.data[5] +
		t14*rotmat.data[6];
	iitWorld.data[2] = t4*rotmat.data[8] +
		t9*rotmat.data[9] +
		t14*rotmat.data[10];
	iitWorld.data[3] = t28*rotmat.data[0] +
		t33*rotmat.data[1] +
		t38*rotmat.data[2];
	iitWorld.data[4] = t28*rotmat.data[4] +
		t33*rotmat.data[5] +
		t38*rotmat.data[6];
	iitWorld.data[5] = t28*rotmat.data[8] +
		t33*rotmat.data[9] +
		t38*rotmat.data[10];
	iitWorld.data[6] = t52*rotmat.data[0] +
		t57*rotmat.data[1] +
		t62*rotmat.data[2];
	iitWorld.data[7] = t52*rotmat.data[4] +
		t57*rotmat.data[5] +
		t62*rotmat.data[6];
	iitWorld.data[8] = t52*rotmat.data[8] +
		t57*rotmat.data[9] +
		t62*rotmat.data[10];
}

void RigidBody::calculateDerivedData()
{
	_calculateTransformMatrix(transformMatrix, position, orientation);
	// Calculate the inertiaTensor in world space.
	_transformInertiaTensor(inverseInertiaTensorWorld,
		orientation,
		inverseInertiaTensor,
		transformMatrix);
}

Vector3 RigidBody::getPointInLocalSpace(const Vector3& point) const
{
	return transformMatrix.transformInverse(point);
}

Vector3 RigidBody::getPointInWorldSpace(const Vector3& point) const
{
	return transformMatrix.transform(point);
}

void RigidBody::setMass(real mass)
{
	assert(mass != 0);
	this->mass = mass;
	inverseMass = ((real)1) / mass;

}


/**
* Internal function that checks the validity of an inverse inertia tensor.
*/
static inline void _checkInverseInertiaTensor(const Matrix3 &iitWorld)
{
	// TODO: Perform a validity check in an assert.
}


void RigidBody::getInertiaTensor(Matrix3 *inertiaTensor) const

{
	inertiaTensor->setInverse(inverseInertiaTensor);
}

Matrix3 RigidBody::getInertiaTensor() const
{
	Matrix3 it;
	getInertiaTensor(&it);
	return it;
}

void RigidBody::getInertiaTensorWorld(Matrix3 *inertiaTensor) const
{
	inertiaTensor->setInverse(inverseInertiaTensorWorld);
}

Matrix3 RigidBody::getInertiaTensorWorld() const
{
	Matrix3 it;
	getInertiaTensorWorld(&it);
	return it;
}

void RigidBody::setInverseInertiaTensor(const Matrix3 &inverseInertiaTensor)
{
	_checkInverseInertiaTensor(inverseInertiaTensor);
	RigidBody::inverseInertiaTensor = inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensor(Matrix3 *inverseInertiaTensor) const
{
	*inverseInertiaTensor = RigidBody::inverseInertiaTensor;
}

Matrix3 RigidBody::getInverseInertiaTensor() const
{
	return inverseInertiaTensor;
}

void RigidBody::getInverseInertiaTensorWorld(Matrix3 *inverseInertiaTensor) const
{
	*inverseInertiaTensor = inverseInertiaTensorWorld;
}

Matrix3 RigidBody::getInverseInertiaTensorWorld() const
{
	return inverseInertiaTensorWorld;
}

void RigidBody::setDamping(const real linearDamping,
	const real angularDamping)
{
	RigidBody::linearDamping = linearDamping;
	RigidBody::angularDamping = angularDamping;
}

void RigidBody::setLinearDamping(const real linearDamping)
{
	RigidBody::linearDamping = linearDamping;
}

real RigidBody::getLinearDamping() const
{
	return linearDamping;
}

void RigidBody::setAngularDamping(const real angularDamping)
{
	RigidBody::angularDamping = angularDamping;
}

real RigidBody::getAngularDamping() const
{
	return angularDamping;
}

void RigidBody::setPosition(const Vector3 &position)
{
	RigidBody::position = position;
}

void RigidBody::setPosition(const real x, const real y, const real z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void RigidBody::getPosition(Vector3 *position) const
{
	*position = RigidBody::position;
}

Vector3 RigidBody::getPosition() const
{
	return position;
}

void RigidBody::setOrientation(const Quaternion &orientation)
{
	RigidBody::orientation = orientation;
	RigidBody::orientation.normalize();
}

void RigidBody::setOrientation(const real r, const real i,
	const real j, const real k)
{
	orientation.r = r;
	orientation.i = i;
	orientation.j = j;
	orientation.k = k;
	orientation.normalize();
}

void RigidBody::getOrientation(Quaternion *orientation) const
{
	*orientation = RigidBody::orientation;
}

Quaternion RigidBody::getOrientation() const
{
	return orientation;
}

void RigidBody::getOrientation(Matrix3 *matrix) const
{
	getOrientation(matrix->data);
}

void RigidBody::getOrientation(real matrix[9]) const
{
	matrix[0] = transformMatrix.data[0];
	matrix[1] = transformMatrix.data[1];
	matrix[2] = transformMatrix.data[2];

	matrix[3] = transformMatrix.data[4];
	matrix[4] = transformMatrix.data[5];
	matrix[5] = transformMatrix.data[6];

	matrix[6] = transformMatrix.data[8];
	matrix[7] = transformMatrix.data[9];
	matrix[8] = transformMatrix.data[10];
}

void RigidBody::getTransform(Matrix4 *transform) const
{
	memcpy(transform, &transformMatrix.data, sizeof(Matrix4));
}

void RigidBody::getTransform(real matrix[16]) const
{
	memcpy(matrix, transformMatrix.data, sizeof(real) * 12);
	matrix[12] = matrix[13] = matrix[14] = 0;
	matrix[15] = 1;
}

void RigidBody::getGLTransform(float matrix[16]) const
{
	matrix[0] = (float)transformMatrix.data[0];
	matrix[1] = (float)transformMatrix.data[4];
	matrix[2] = (float)transformMatrix.data[8];
	matrix[3] = 0;

	matrix[4] = (float)transformMatrix.data[1];
	matrix[5] = (float)transformMatrix.data[5];
	matrix[6] = (float)transformMatrix.data[9];
	matrix[7] = 0;

	matrix[8] = (float)transformMatrix.data[2];
	matrix[9] = (float)transformMatrix.data[6];
	matrix[10] = (float)transformMatrix.data[10];
	matrix[11] = 0;

	matrix[12] = (float)transformMatrix.data[3];
	matrix[13] = (float)transformMatrix.data[7];
	matrix[14] = (float)transformMatrix.data[11];
	matrix[15] = 1;
}

void RigidBody::setVelocity(const Vector3 &velocity)
{
	RigidBody::velocity = velocity;
}

void RigidBody::setVelocity(const real x, const real y, const real z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

void RigidBody::getVelocity(Vector3 *velocity) const
{
	*velocity = RigidBody::velocity;
}

Vector3 RigidBody::getVelocity() const
{
	return velocity;
}

void RigidBody::addVelocity(const Vector3 &deltaVelocity)
{
	velocity += deltaVelocity;
}

void RigidBody::setRotation(const Vector3 &rotation)
{
	RigidBody::rotation = rotation;
}

void RigidBody::setRotation(const real x, const real y, const real z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}

void RigidBody::getRotation(Vector3 *rotation) const
{
	*rotation = RigidBody::rotation;
}

Vector3 RigidBody::getRotation() const
{
	return rotation;
}

void RigidBody::addRotation(const Vector3 &deltaRotation)
{
	rotation += deltaRotation;
}

void RigidBody::setAwake(const bool awake)
{
	if (awake) {
		isAwake = true;
		// Add a bit of motion to avoid it falling asleep immediately.
		motion = sleepEpsilon*2.0f;
	}
	else {
		isAwake = false;
		velocity.clear();
		rotation.clear();
	}
}

Vector3 RigidBody::getDirectionInLocalSpace(const Vector3 &direction) const
{
	return transformMatrix.transformInverseDirection(direction);
}

void RigidBody::setCanSleep(const bool canSleep)
{
	RigidBody::canSleep = canSleep;

	if (!canSleep && !isAwake) setAwake();
}

void RigidBody::getLastFrameAcceleration(Vector3 *acceleration) const
{
	*acceleration = lastFrameAcceleration;
}

Vector3 RigidBody::getLastFrameAcceleration() const
{
	return lastFrameAcceleration;
}