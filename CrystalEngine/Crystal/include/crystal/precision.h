#pragma once
#include <math.h>
#include <float.h>
#include <string>
namespace crystal {

	/**
	* Define a real number precision.
	* By default single precision is used.
	*/

	typedef float real;
	#define REAL_MAX FLT_MAX

	/** Defines the precision of the square root operator. */ 
	#define real_sqrt sqrt
	#define real_pow pow
	#define real_abs fabs
	#define real_epsilon DBL_EPSILON
	#define R_PI 3.14159265358979
	#define real_sin sinf
	#define real_cos cosf
	#define real_asin asinf
	#define real_acos acosf
	#define defalut_gravity -9.8f
	typedef std::string String;

	static real radians(real degree)
	{
		return degree /((real)180.f)*R_PI;
	}
}