#include "EagleEngineUtil.h"

namespace ee
{
	const std::string IntegerToString(int i)
	{
		std::ostringstream ss;
		ss << i;
		return ss.str();
	}

	std::string FloatToString(float f)
	{
		std::ostringstream ss;
		ss << f;
		return ss.str();
	}

	const std::string DoubleToString(double d)
	{
		std::ostringstream ss;
		ss << d;
		return ss.str();
	}

	const int StringToInteger(const std::string &string)
	{
		std::istringstream iss(string);
		int result;
		return iss >> result ? result : 0;
	}
}