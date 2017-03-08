#include <crystal/fgen.h>

using namespace crystal;

void ForceRegistry::updateForces(real duration)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); i++)
	{
		i->fg->updateForce(i->body, duration);
	}
}

void ForceRegistry::add(RigidBody *body, ForceGenerator *fg)
{
	ForceRegistry::ForceRegistration registration;
	registration.body = body;
	registration.fg = fg;
	registrations.push_back(registration);
}

void ForceRegistry::remove(RigidBody* body, ForceGenerator *fg)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); i++)
	{
		if (i->body == body && i->fg == fg)
		{
			registrations.erase(i);
		}
	}
}

void ForceRegistry::clear()
{
	registrations.clear();
}

void Gravity::updateForce(RigidBody *body, real duration)
{
	if (body->hasFiniteMass()) 
	{
		body->addForce(gravity * body->getMass());
	}
	
}

void ConstantForce::updateForce(RigidBody *body, real duration)
{
	if (body->hasFiniteMass())
	{
		body->addForce(force * body->getMass());
	}
}

void Spring::updateForce(RigidBody *body, real duration)
{
	Vector3 pw = body->getPointInWorldSpace(connectionPoint);
	Vector3 force =  other->getPointInWorldSpace(otherConnectionPoint) - pw;
	real magnitude = force.magnitude();
	magnitude = real_abs(magnitude - restLength) * springConstant;
	force.normalize();
	force = force * magnitude;
	body->addForceAtPoint(force , pw);
}

Aero::Aero(const Matrix3 &tensor, const Vector3 &position, const Vector3 *windspeed)
{
	Aero::tensor = tensor;
	Aero::position = position;
	Aero::windspeed = windspeed;
}

void Aero::updateForce(RigidBody *body, real duration)
{
	Aero::updateForceFromTensor(body, duration, tensor);
}

void Aero::updateForceFromTensor(RigidBody *body, real duration,
	const Matrix3 &tensor)
{
	// Calculate total velocity (windspeed and body's velocity).
	Vector3 velocity = body->getVelocity();
	velocity += *windspeed;

	// Calculate the velocity in body coordinates
	Vector3 bodyVel = body->getTransform().transformInverseDirection(velocity);

	// Calculate the force in body coordinates
	Vector3 bodyForce = tensor.transform(bodyVel);
	Vector3 force = body->getTransform().transformDirection(bodyForce);
	// Apply the force
	body->addForceAtBodyPoint(force, position);
}



AeroControl::AeroControl(const Matrix3 &base, const Matrix3 &min, const Matrix3 &max,

	const Vector3 &position, const Vector3 *windspeed)

	:

	Aero(base, position, windspeed)

{

	AeroControl::minTensor = min;

	AeroControl::maxTensor = max;

	controlSetting = 0.0f;

}



Matrix3 AeroControl::getTensor()

{

	if (controlSetting <= -1.0f) return minTensor;

	else if (controlSetting >= 1.0f) return maxTensor;

	else if (controlSetting < 0)

	{

		return Matrix3::linearInterpolate(minTensor, tensor, controlSetting + 1.0f);

	}

	else if (controlSetting > 0)

	{

		return Matrix3::linearInterpolate(tensor, maxTensor, controlSetting);

	}

	else return tensor;

}



void AeroControl::setControl(real value)

{

	controlSetting = value;

}



void AeroControl::updateForce(RigidBody *body, real duration)

{

	Matrix3 tensor = getTensor();

	Aero::updateForceFromTensor(body, duration, tensor);

}