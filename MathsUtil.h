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

		static XMMATRIX InverseTranspose(CXMMATRIX M)
		{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
		}
	}
}

#endif