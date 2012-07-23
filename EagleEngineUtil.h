#pragma once

#ifndef _EE_UTIL_H_
#define _EE_UTIL_H_

#include <string>
#include <Windows.h>
#include <sstream>

namespace ee
{
	const std::string IntegerToString(int i);

	std::string FloatToString(float f);

	const std::string DoubleToString(double d);

	const int StringToInteger(const std::string &string);

	const std::wstring ts2ws(const std::basic_string<TCHAR> &s);
}

#endif