#pragma once
#include "precision.h"
namespace crystal {
	
	/**
	* Holds the value for energy under which a body will be put to
	* sleep. This is a global value for the whole solution.  By
	* default it is 0.1, which is fine for simulation when gravity is
	* about 20 units per second squared, masses are about one, and
	* other forces are around that of gravity. It may need tweaking
	* if your simulation is drastically different to this.
	*/
	extern real sleepEpsilon;

	/**
	* Sets the current sleep epsilon value: the kinetic energy under
	* which a body may be put to sleep. Bodies are put to sleep if
	* they appear to have a stable kinetic energy less than this
	* value. For simulations that often have low values (such as slow
	* moving, or light objects), this may need reducing.
	*
	* The value is global; all bodies will use it.
	*
	* @see sleepEpsilon
	*
	* @see getSleepEpsilon
	*
	* @param value The sleep epsilon value to use from this point
	* on.
	*/
	void setSleepEpsilon(real value);
	
	/**
	* Gets the current value of the sleep epsilon parameter.
	*
	* @see sleepEpsilon
	*
	* @see setSleepEpsilon
	*
	* @return The current value of the parameter.
	*/
	real getSleepEpsilon();

	/**
	* Represent a three dimension vector.
	*/
	class Vector3 {

	public:
		real x;
		real y;
		real z;

		const static Vector3 GRAVITY;
		const static Vector3 HIGH_GRAVITY;
		const static Vector3 UP;
		const static Vector3 RIGHT;
		const static Vector3 OUT_OF_SCREEN;
		const static Vector3 X;
		const static Vector3 Y;
		const static Vector3 Z;
	private:
		/*  Padding to ensure 4-word alignment.  */
		real padding;

	public:

		Vector3() :x(0), y(0), z(0) {}

		Vector3(const real x, const real y, const real z) :x(x), y(y), z(z) {}

		void invert() 
		{
			x = -x;
			y = -y;
			z = -z;
		}

		/* Get magnitude */
		real magnitude() const
		{
			return real_sqrt(x*x + y*y + z*z);
		}

		/* Get squared magnitude */
		real squaredMagnitude() const 
		{
			return x*x + y*y + z*z;
		}

		/* Turns a non-zero vector into a vector of unit length */
		void normalize()
		{
			real length = magnitude();
			if (length > 0) {
				(*this) *= ((real)1)/ length;
			}
		}

		void getNormalize(Vector3& vector) const
		{
			real length = magnitude();
			vector.x = x / length;
			vector.y = y / length;
			vector.z = z / length;
		}

		/* Multiplies this vector by the given scalar */
		void operator *= (const real scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
		}
       
		/** Returns a copy of this vector scaled to the given value. */
		Vector3 operator * (const real scalar) const
		{
			return Vector3(x*scalar, y*scalar, z*scalar);
		}

		/* Add a vector */
		void operator += (const Vector3& vector)
		{
			x += vector.x;
			y += vector.y;
			z += vector.z;
		}

		/* Return a copy of the result of addition */
		Vector3 operator +(const Vector3& vector) const
		{
			return Vector3(x + vector.x, y + vector.y, z + vector.z);
		}

		/* Substract a vector */
		void operator -= (const Vector3& vector)
		{
			x -= vector.x;
			y -= vector.y;
			z -= vector.z;
		}

		/* Return a copy of the result of substraction */
		Vector3 operator -(const Vector3& vector) const
		{
			return Vector3(x - vector.x, y - vector.y, z - vector.z);
		}

		real operator[](unsigned i) const
		{
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		real& operator[](unsigned i)
		{
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		/* Add a scaled vector */
		void addScaledVector(const Vector3& vector, real scalar)
		{
			x += vector.x*scalar;
			y += vector.y*scalar;
			z += vector.z*scalar;
		}

		/* A component-wise product of two vectors. return a new vector
		 * Not commonly used
		 */
		Vector3 componentProduct(const Vector3& vector) const
		{
			return Vector3(x*vector.x, y*vector.y, z*vector.z);
		}

		/* A component-wise product of two vectors. Update the current vector to its result
		* Not commonly used
		*/
		void componentProductUpdate(const Vector3& vector)
		{
			x *= vector.x;
			y *= vector.y;
			z *= vector.z;
		}

		/* Calculate the dot(inner) product of two vectors */
		real scalarProduct(Vector3& vector) const
		{
			return x*vector.x + y*vector.y + z*vector.z;
		}

		/* Calculate the dot(inner) product of two vectors */
		real operator *(const Vector3& vector) const
		{
			return x*vector.x + y*vector.y + z*vector.z;
		}

		/* Calcute the vector(cross) product of two vectors.*/
		Vector3 vectorProduct(const Vector3& vector) const
		{
			return Vector3(y*vector.z-z*vector.y,
				           z*vector.x-x*vector.z,
				           x*vector.y-y*vector.x);
		}

		/* Calcute the vector(cross) product of two vectors.
		 * Overload operator % just for convenience
		 */
		void operator %=(const Vector3& vector)
		{
			*this = vectorProduct(vector);
		}

		/* Calcute the vector(cross) product of two vectors.
		 * Overload operator % just for convenience
		 */
		Vector3 operator %(const Vector3& vector) const 
		{
			return Vector3(y*vector.z - z*vector.y,
				           z*vector.x - x*vector.z,
				           x*vector.y - y*vector.x);
		}

		/* Zero all the components of the vector. */
		void clear()
		{
			x = y = z = 0;
		}

	};

	/**
	 * A 3*3 matrix
	 * Holds an inertia tensor, consisting of a 3x3 row-major matrix.
 	 * This matrix is not padding to produce an aligned structure, since
	 * it is most commonly used with a mass (single real) and two
	 * damping coefficients to make the 12-element characteristics array
	 * of a rigid body.
	 */
	class Matrix3
	{
	public:
		//Holds data in array form
		real data[9];

		/**
		* Transform the given vector by this matrix.
		*
		* @param vector The vector to transform.
		*/
		Vector3 operator* (const Vector3& vector) const
		{
			return Vector3{
				vector.x * data[0] + vector.y * data[1] + vector.z * data[2],
				vector.x * data[3] + vector.y * data[4] + vector.z * data[5],
				vector.x * data[6] + vector.y * data[7] + vector.z * data[8]
			};
		}

		/**
		 * Combine two transformation
		 */
		Matrix3 operator* (const Matrix3& o) const
		{
			return Matrix3(
				data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6],
				data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7],
				data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8],

				data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6],
				data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7],
				data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8],

				data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6],
				data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7],
				data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8]

			);
		}

		/**
		* Sets the matrix to be the inverse of the given matrix.
		*
		* @param m The matrix to invert and use to set this.
		*/
		void setInverse(const Matrix3 &m);

		/** Returns a new matrix containing the inverse of this matrix. */
		Matrix3 inverse() const
		{
			Matrix3 result;
			result.setInverse(*this);
			return result;
		}

		/**
		* Inverts the matrix.
		*/
		void invert()
		{
			setInverse(*this);
		}

		/** 
		* Sets the matrix to be the transpose of the given matrix.
		* If the matrix represents a rotation, then transpose has same effect as invert
		* @param m The matrix to transpose and use to set this. 
 		*/ 
		void setTranspose(const Matrix3 &m)
		{ 
			data[0] = m.data[0]; 
			data[1] = m.data[3]; 
			data[2] = m.data[6]; 
			data[3] = m.data[1];
			data[4] = m.data[4];
			data[5] = m.data[7]; 
			data[6] = m.data[2]; 
			data[7] = m.data[5]; 
			data[8] = m.data[8]; 
		}

		/* Returns a new matrix containing the transpose of this matrix. */
		Matrix3 transpose()
		{
			Matrix3 result;
			result.setTranspose(*this);
			return result;
		}

		/**
		* Transform the given vector by this matrix.
		*
		* @param vector The vector to transform.
		*/
		Vector3 transform(const Vector3 &vector) const
		{
			return (*this) * vector;
		}

		/**
		* Creates a new matrix with explicit coefficients.
		*/
		Matrix3(real c0, real c1, real c2, real c3, real c4, real c5,
			real c6, real c7, real c8)
		{
			data[0] = c0; data[1] = c1; data[2] = c2;
			data[3] = c3; data[4] = c4; data[5] = c5;
			data[6] = c6; data[7] = c7; data[8] = c8;
		}

		/**
		* Creates a new matrix.
		*/
		Matrix3()
		{
			data[0] = data[1] = data[2] = data[3] = data[4] = data[5] =
				data[6] = data[7] = data[8] = 0;
		}

		/**
		* Sets the value of the matrix as an inertia tensor of
		* a rectangular block aligned with the body's coordinate
		* system with the given axis half-sizes and mass.
		*/
		void setBlockInertiaTensor(const Vector3 &halfSizes, real mass)
		{
			Vector3 squares = halfSizes.componentProduct(halfSizes);
			setInertiaTensorCoeffs(0.3f*mass*(squares.y + squares.z),
				0.3f*mass*(squares.x + squares.z),
				0.3f*mass*(squares.x + squares.y));
		}


		/**
		* Sets the value of the matrix from inertia tensor values.
		*/
		void setInertiaTensorCoeffs(real ix, real iy, real iz,
			real ixy = 0, real ixz = 0, real iyz = 0)
		{
			data[0] = ix;
			data[1] = data[3] = -ixy;
			data[2] = data[6] = -ixz;
			data[4] = iy;
			data[5] = data[7] = -iyz;
			data[8] = iz;
		}

		/**
		* Does a component-wise addition of this matrix and the given
		* matrix.
		*/
		void operator+=(const Matrix3 &o)
		{
			data[0] += o.data[0]; data[1] += o.data[1]; data[2] += o.data[2];
			data[3] += o.data[3]; data[4] += o.data[4]; data[5] += o.data[5];
			data[6] += o.data[6]; data[7] += o.data[7]; data[8] += o.data[8];
		}

		/**
		* Sets the matrix values from the given three vector components.
		* These are arranged as the three columns of the vector.
		*/
		void setComponents(const Vector3 &compOne, const Vector3 &compTwo,
			const Vector3 &compThree)
		{
			data[0] = compOne.x;
			data[1] = compTwo.x;
			data[2] = compThree.x;
			data[3] = compOne.y;
			data[4] = compTwo.y;
			data[5] = compThree.y;
			data[6] = compOne.z;
			data[7] = compTwo.z;
			data[8] = compThree.z;

		}

		/**
		* Transform the given vector by the transpose of this matrix.
		*
		* @param vector The vector to transform.
		*/
		Vector3 transformTranspose(const Vector3 &vector) const
		{
			return Vector3(
				vector.x * data[0] + vector.y * data[3] + vector.z * data[6],
				vector.x * data[1] + vector.y * data[4] + vector.z * data[7],
				vector.x * data[2] + vector.y * data[5] + vector.z * data[8]
			);
		}

		/**
		* Sets the matrix to be a skew symmetric matrix based on
		* the given vector. The skew symmetric matrix is the equivalent
		* of the vector product. So if a,b are vectors. a x b = A_s b
		* where A_s is the skew symmetric form of a.
		*/
		void setSkewSymmetric(const Vector3 vector)
		{
			data[0] = data[4] = data[8] = 0;
			data[1] = -vector.z;
			data[2] = vector.y;
			data[3] = vector.z;
			data[5] = -vector.x;
			data[6] = -vector.y;
			data[7] = vector.x;
		}

		/**
		* Multiplies this matrix in place by the given other matrix.
		*/
		void operator*=(const Matrix3 &o)
		{
			real t1;
			real t2;
			real t3;

			t1 = data[0] * o.data[0] + data[1] * o.data[3] + data[2] * o.data[6];
			t2 = data[0] * o.data[1] + data[1] * o.data[4] + data[2] * o.data[7];
			t3 = data[0] * o.data[2] + data[1] * o.data[5] + data[2] * o.data[8];
			data[0] = t1;
			data[1] = t2;
			data[2] = t3;

			t1 = data[3] * o.data[0] + data[4] * o.data[3] + data[5] * o.data[6];
			t2 = data[3] * o.data[1] + data[4] * o.data[4] + data[5] * o.data[7];
			t3 = data[3] * o.data[2] + data[4] * o.data[5] + data[5] * o.data[8];
			data[3] = t1;
			data[4] = t2;
			data[5] = t3;

			t1 = data[6] * o.data[0] + data[7] * o.data[3] + data[8] * o.data[6];
			t2 = data[6] * o.data[1] + data[7] * o.data[4] + data[8] * o.data[7];
			t3 = data[6] * o.data[2] + data[7] * o.data[5] + data[8] * o.data[8];
			data[6] = t1;
			data[7] = t2;
			data[8] = t3;
		}

		/**
		* Multiplies this matrix in place by the given scalar.
		*/
		void operator*=(const real scalar)
		{
			data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
			data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
			data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
		}

		/**
		* Sets the matrix to be a diagonal matrix with the given
		* values along the leading diagonal.
		*/
		void setDiagonal(real a, real b, real c)
		{
			setInertiaTensorCoeffs(a, b, c);
		}

		/**
		* Interpolates a couple of matrices.
		*/
		static Matrix3 linearInterpolate(const Matrix3& a, const Matrix3& b, real prop);

	};

	/**
	 * A 3*4 matix
	 * Holds a transform matrix, consisting of a rotation matrix and
	 * a position. The matrix has 12 elements, it is assumed that the
	 * remaining four are (0,0,0,1); producing a homogenous matrix.
	 */
	class Matrix4
	{
	public:
		real data[12];
		/**
		* Transform the given vector by this matrix.
		*
		* @param vector The vector to transform.
		*/
		Vector3 operator* (const Vector3& vector) const
		{
			
			return Vector3
			{
				vector.x * data[0] +
				vector.y * data[1] +
				vector.z * data[2] + data[3],

				vector.x * data[4] +
				vector.y * data[5] +
				vector.z * data[6] + data[7],

				vector.x * data[8] +
				vector.y * data[9] +
				vector.z * data[10] + data[11]
			};
		}


		/**
		* Returns a matrix which is this matrix multiplied by the given
		* other matrix.
		*/
		Matrix4 operator* (const Matrix4& o) const;

		real getDeterminant() const
		{
			return -data[8] * data[5] * data[2] +
				data[4] * data[9] * data[2] +
				data[8] * data[1] * data[6] -
				data[0] * data[9] * data[6] -
				data[4] * data[1] * data[10] +
				data[0] * data[5] * data[10];
		}

		void setInverse(const Matrix4 &m);

		/* Returns a new matrix containing the inverse of this matrix. */
		Matrix4 inverse() const
		{
			Matrix4 result;
			result.setInverse(*this);
			return result;
		}

		/* Invert the matrix */
		void invert()
		{
			setInverse(*this);
		}

		/**
		* Gets a vector representing one axis (i.e. one column) in the matrix.
		*
		* @param i The row to return. Row 3 corresponds to the position
		* of the transform matrix.
		*
		* @return The vector.
		*/
		Vector3 getAxisVector(int i) const
		{
			return Vector3(data[i], data[i + 4], data[i + 8]);
		}

		/**
		* Creates an identity matrix.
		*/
		Matrix4()
		{
			data[1] = data[2] = data[3] = data[4] = data[6] =
				data[7] = data[8] = data[9] = data[11] = 0;
			data[0] = data[5] = data[10] = 1;
		}

		/**
		* Transform the given vector by this matrix.
		*
		* @param vector The vector to transform.
		*/
		Vector3 transform(const Vector3 &vector) const
		{
			return (*this) * vector;
		}

		/**
		* Transform the given vector by the transformational inverse
		* of this matrix.
		*
		* @note This function relies on the fact that the inverse of
		* a pure rotation matrix is its transpose. It separates the
		* translational and rotation components, transposes the
		* rotation, and multiplies out. If the matrix is not a
		* scale and shear free transform matrix, then this function
		* will not give correct results.
		*
		* @param vector The vector to transform.
		*/
		Vector3 transformInverse(const Vector3 &vector) const
		{
			Vector3 tmp = vector;
			tmp.x -= data[3];
			tmp.y -= data[7];
			tmp.z -= data[11];

			return Vector3(
				tmp.x * data[0] +
				tmp.y * data[4] +
				tmp.z * data[8],

				tmp.x * data[1] +
				tmp.y * data[5] +
				tmp.z * data[9],

				tmp.x * data[2] +
				tmp.y * data[6] +
				tmp.z * data[10]
			);
		}

		/**
		* Transform the given direction vector by this matrix.
		*
		* @note When a direction is converted between frames of
		* reference, there is no translation required.
		*
		* @param vector The vector to transform.
		*/
		Vector3 transformDirection(const Vector3 &vector) const
		{
			return Vector3(
				vector.x * data[0] +
				vector.y * data[1] +
				vector.z * data[2],

				vector.x * data[4] +
				vector.y * data[5] +
				vector.z * data[6],

				vector.x * data[8] +
				vector.y * data[9] +
				vector.z * data[10]
			);
		}

		/**
		* Transform the given direction vector by the
		* transformational inverse of this matrix.
		*
		* @note This function relies on the fact that the inverse of
		* a pure rotation matrix is its transpose. It separates the
		* translational and rotation components, transposes the
		* rotation, and multiplies out. If the matrix is not a
		* scale and shear free transform matrix, then this function
		* will not give correct results.
		*
		* @note When a direction is converted between frames of
		* reference, there is no translation required.
		*
		* @param vector The vector to transform.
		*/
		Vector3 transformInverseDirection(const Vector3 &vector) const
		{
			return Vector3(
				vector.x * data[0] +
				vector.y * data[4] +
				vector.z * data[8],

				vector.x * data[1] +
				vector.y * data[5] +
				vector.z * data[9],

				vector.x * data[2] +
				vector.y * data[6] +
				vector.z * data[10]
			);
		}


		/**
		* Fills the given array with this transform matrix, so it is
		* usable as an open-gl transform matrix. OpenGL uses a column
		* major format, so that the values are transposed as they are
		* written.
		*/
		void fillGLArray(float array[16]) const
		{
			array[0] = (float)data[0];
			array[1] = (float)data[4];
			array[2] = (float)data[8];
			array[3] = (float)0;

			array[4] = (float)data[1];
			array[5] = (float)data[5];
			array[6] = (float)data[9];
			array[7] = (float)0;

			array[8] = (float)data[2];
			array[9] = (float)data[6];
			array[10] = (float)data[10];
			array[11] = (float)0;

			array[12] = (float)data[3];
			array[13] = (float)data[7];
			array[14] = (float)data[11];
			array[15] = (float)1;
		}
	};

	/**
	* Holds a three degree of freedom orientation.
	*
	* Quaternions have
	* several mathematical properties that make them useful for
	* representing orientations, but require four items of data to
	* hold the three degrees of freedom. These four items of data can
	* be viewed as the coefficients of a complex number with three
	* imaginary parts. The mathematics of the quaternion is then
	* defined and is roughly correspondent to the math of 3D
	* rotations. A quaternion is only a valid rotation if it is
	* normalised: i.e. it has a length of 1.
	*
	* @note Angular velocity and acceleration can be correctly
	* represented as vectors. Quaternions are only needed for
	* orientation.
	*/
	class Quaternion
	{
	public:
		union {
			struct {
				/**
				* Holds the real component of the quaternion.
				*/
				real r;

				/**
				* Holds the first complex component of the
				* quaternion.
				*/
				real i;

				/**
				* Holds the second complex component of the
				* quaternion.
				*/
				real j;

				/**
				* Holds the third complex component of the
				* quaternion.
				*/
				real k;
			};
			/**
			* Holds the quaternion data in array form.
			*/
			real data[4];
		};

		//Default constructer. A quaternion representing no rotation
		Quaternion():r(1),i(0),j(0),k(0){}
		/**
		* The explicit constructor creates a quaternion with the given
		* components.
		*/
		Quaternion(const real r, const real i, const real j, const real k) :
			r(r), i(i), j(j), k(k) {}

		/**
		* Construct a quaternion with an axis and degree. 
		* Rotate clockwise around the axis
		*/
		Quaternion(real degree, const Vector3& axis)
		{
			Vector3 normalA;
			axis.getNormalize(normalA);
			r = real_cos(radians(degree/ (real)2.0f));
			real sinD = real_sin(radians(degree/(real)2.0f));
			i = normalA.x * sinD;
			j = normalA.y * sinD;
			k = normalA.z * sinD;
		}

		//Check whether the quaternion represents no ratation
		bool noRoatation()
		{
			return (i == j == k == 0);
		}

		/**
		* Normalises the quaternion to unit length, making it a valid
		* orientation quaternion.
		*/
		void normalize()
		{
			real d = r*r + i*i + j*j + k*k;
			// Check for zero length quaternion, and use the no-rotation
			// quaternion in that case.
			if (d < real_epsilon) {
				r = 1;
				return;
			}
			d = ((real)1.0)/ real_sqrt(d);

			r = r*d;
			i = i*d;
			j = j*d;
			k = k*d;

		}

		/** 
		 * Multiplies the quaternion by the given quaternion. 
		 *
		 * @param multiplier The quaternion by which to multiply. 
		 */

		void operator *= (const Quaternion& q)
		{
			r = r*q.r - i*q.i - j*q.j - k*q.k;
			i = r*q.i + i*q.r + j*q.k - k*q.j;
			j = r*q.j - i*q.k + j*q.r + k*q.i;
			k = r*q.k + i*q.j - j*q.i + k*q.r;
		}

		/**
		* Adds the given vector to this, scaled by the given amount.
		* This is used to update the orientation quaternion by a rotation
		* and time.
		*
		* @param vector The vector to add.
		*
		* @param scale The amount of the vector to add.
		*/
		void addScaledVector(const Vector3& vector, real scale)
		{
			Quaternion q(0,
				vector.x * scale,
				vector.y * scale,
				vector.z * scale);
			q *= *this;
			r += q.r * ((real)0.5);
			i += q.i * ((real)0.5);
			j += q.j * ((real)0.5);
			k += q.k * ((real)0.5);
		}

		void rotateByVector(const Vector3& vector)
		{
			Quaternion q(0, vector.x, vector.y, vector.z);
			(*this) *= q;
		}
	};
}