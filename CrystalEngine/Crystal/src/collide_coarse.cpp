#include <crystal/collide_coarse.h>

using namespace crystal;

BoundingSphere::BoundingSphere(const BoundingSphere& bs1, const BoundingSphere& bs2)
{
	Vector3 centerOffset = bs2.center - bs1.center;
	real distance = centerOffset.squaredMagnitude();
	real radiusDiff = bs2.radius - bs1.radius;

	// Check if the larger sphere encloses the small one
	if (radiusDiff*radiusDiff >= distance)
	{
		if (bs1.radius > bs2.radius)
		{
			center = bs1.center;
			radius = bs1.radius;
		}
		else
		{
			center = bs2.center;
			radius = bs2.radius;
		}
	}

	// Otherwise we need to work with partially
	// overlapping spheres
	else
	{
		distance = real_sqrt(distance);
		radius = (distance + bs1.radius + bs2.radius) * ((real)0.5);

		// The new center is based on bs1's center, moved towards
		// bs2's center by an ammount proportional to the spheres'
		// radius.
		center = bs1.center;
		if (distance > 0)
		{
			center += centerOffset * ((radius - bs1.radius) / distance);
		}
	}
}

bool BoundingSphere::overlaps(const BoundingSphere& other) const
{
	real distanceSquared = (center - other.center).squaredMagnitude();
	return distanceSquared < (radius + other.radius)*(radius + other.radius);
}

real BoundingSphere::getGrowth(const BoundingSphere& other) const
{
	BoundingSphere newSphere = BoundingSphere{ *this,other };
	// return newSphere.radius*newSphere.radius - radius*radius;
	return newSphere.radius - radius;
}