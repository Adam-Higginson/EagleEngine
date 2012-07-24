//Helper class for some mathematical functions
#pragma once

#ifndef _MATHS_UTIL_H_
#define _MATHS_UTIL_H_


namespace ee
{
	namespace MathsUtil
	{
		const float PI = 3.14159265f;

		template<typename T>
		static T Clamp(const T& x, const T& low, const T& high)
		{
			return x < low ? low : (x > high ? high : x);
		}
	}
}

#endif