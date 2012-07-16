#pragma once

#ifndef _EE_UTIL_H_
#define _EE_UTIL_H_

#include <string>
#include <sstream>

namespace ee
{
	const std::string IntegerToString(int i);

	std::string FloatToString(float f);

	const std::string DoubleToString(double d);

	const int StringToInteger(const std::string &string);
}

#endif