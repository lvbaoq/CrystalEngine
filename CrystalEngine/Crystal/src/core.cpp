#include <crystal/core.h>

using namespace crystal;

/*
* Definition of the sleep epsilon extern.
*/
real crystal::sleepEpsilon = ((real)0.3);

const Vector3 Vector3::GRAVITY = Vector3(0, -9.81, 0);
const Vector3 Vector3::HIGH_GRAVITY = Vector3(0, -19.62, 0);
const Vector3 Vector3::UP = Vector3(0, 1, 0);
const Vector3 Vector3::RIGHT = Vector3(1, 0, 0);
const Vector3 Vector3::OUT_OF_SCREEN = Vector3(0, 0, 1);
const Vector3 Vector3::X = Vector3(0, 1, 0);
const Vector3 Vector3::Y = Vector3(1, 0, 0);
const Vector3 Vector3::Z = Vector3(0, 0, 1);

void crystal::setSleepEpsilon(real value)
{
	sleepEpsilon = value;
}

real crystal::getSleepEpsilon()
{
	return sleepEpsilon;
}

void Matrix3::setInverse(const Matrix3 &m)

{
	real t4 = m.data[0] * m.data[4];
	real t6 = m.data[0] * m.data[5];
	real t8 = m.data[1] * m.data[3];
	real t10 = m.data[2] * m.data[3];
	real t12 = m.data[1] * m.data[6];
	real t14 = m.data[2] * m.data[6];

	// Calculate the determinant
	real t16 = (t4*m.data[8] - t6*m.data[7] - t8*m.data[8] +
		t10*m.data[7] + t12*m.data[5] - t14*m.data[4]);

	// Make sure the determinant is non-zero.
	if (t16 == (real)0.0f) return;
	real t17 = 1 / t16;

	data[0] = (m.data[4] * m.data[8] - m.data[5] * m.data[7])*t17;
	data[1] = -(m.data[1] * m.data[8] - m.data[2] * m.data[7])*t17;
	data[2] = (m.data[1] * m.data[5] - m.data[2] * m.data[4])*t17;
	data[3] = -(m.data[3] * m.data[8] - m.data[5] * m.data[6])*t17;
	data[4] = (m.data[0] * m.data[8] - t14)*t17;
	data[5] = -(t6 - t10)*t17;
	data[6] = (m.data[3] * m.data[7] - m.data[4] * m.data[6])*t17;
	data[7] = -(m.data[0] * m.data[7] - t12)*t17;
	data[8] = (t4 - t8)*t17;
}

void Matrix4::setInverse(const Matrix4 &m)
{
	// Make sure the determinant is non-zero.
	real det = getDeterminant();

	if (det == 0) return;

	det = ((real)1.0) / det;

	data[0] = (-m.data[9] * m.data[6] + m.data[5] * m.data[10])*det;
	data[4] = (m.data[8] * m.data[6] - m.data[4] * m.data[10])*det;
	data[8] = (-m.data[8] * m.data[5] + m.data[4] * m.data[9])*det;

	data[1] = (m.data[9] * m.data[2] - m.data[1] * m.data[10])*det;
	data[5] = (-m.data[8] * m.data[2] + m.data[0] * m.data[10])*det;
	data[9] = (m.data[8] * m.data[1] - m.data[0] * m.data[9])*det;

	data[2] = (-m.data[5] * m.data[2] + m.data[1] * m.data[6])*det;
	data[6] = (+m.data[4] * m.data[2] - m.data[0] * m.data[6])*det;
	data[10] = (-m.data[4] * m.data[1] + m.data[0] * m.data[5])*det;

	data[3] = (m.data[9] * m.data[6] * m.data[3]
		- m.data[5] * m.data[10] * m.data[3]
		- m.data[9] * m.data[2] * m.data[7]
		+ m.data[1] * m.data[10] * m.data[7]
		+ m.data[5] * m.data[2] * m.data[11]
		- m.data[1] * m.data[6] * m.data[11])*det;

	data[7] = (-m.data[8] * m.data[6] * m.data[3]
		+ m.data[4] * m.data[10] * m.data[3]
		+ m.data[8] * m.data[2] * m.data[7]
		- m.data[0] * m.data[10] * m.data[7]
		- m.data[4] * m.data[2] * m.data[11]
		+ m.data[0] * m.data[6] * m.data[11])*det;

	data[11] = (m.data[8] * m.data[5] * m.data[3]
		- m.data[4] * m.data[9] * m.data[3]
		- m.data[8] * m.data[1] * m.data[7]
		+ m.data[0] * m.data[9] * m.data[7]
		+ m.data[4] * m.data[1] * m.data[11]
		- m.data[0] * m.data[5] * m.data[11])*det;
}

Matrix4 Matrix4::operator* (const Matrix4& o) const
{
	Matrix4 result;
	result.data[0] = (o.data[0] * data[0]) + (o.data[4] * data[1]) + (o.data[8] * data[2]);
	result.data[4] = (o.data[0] * data[4]) + (o.data[4] * data[5]) + (o.data[8] * data[6]);
	result.data[8] = (o.data[0] * data[8]) + (o.data[4] * data[9]) + (o.data[8] * data[10]);

	result.data[1] = (o.data[1] * data[0]) + (o.data[5] * data[1]) + (o.data[9] * data[2]);
	result.data[5] = (o.data[1] * data[4]) + (o.data[5] * data[5]) + (o.data[9] * data[6]);
	result.data[9] = (o.data[1] * data[8]) + (o.data[5] * data[9]) + (o.data[9] * data[10]);

	result.data[2] = (o.data[2] * data[0]) + (o.data[6] * data[1]) + (o.data[10] * data[2]);
	result.data[6] = (o.data[2] * data[4]) + (o.data[6] * data[5]) + (o.data[10] * data[6]);
	result.data[10] = (o.data[2] * data[8]) + (o.data[6] * data[9]) + (o.data[10] * data[10]);

	result.data[3] = (o.data[3] * data[0]) + (o.data[7] * data[1]) + (o.data[11] * data[2]) + data[3];
	result.data[7] = (o.data[3] * data[4]) + (o.data[7] * data[5]) + (o.data[11] * data[6]) + data[7];
	result.data[11] = (o.data[3] * data[8]) + (o.data[7] * data[9]) + (o.data[11] * data[10]) + data[11];

	return result;
}

Matrix3 Matrix3::linearInterpolate(const Matrix3& a, const Matrix3& b, real prop)
{
	Matrix3 result;
	for (unsigned i = 0; i < 9; i++) {
		result.data[i] = a.data[i] * (1 - prop) + b.data[i] * prop;
	}
	return result;
}