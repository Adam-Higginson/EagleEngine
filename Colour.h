//Helper class to help represent colour
#pragma once

#ifndef _H_COLOUR_H_
#define _H_COLOUR_H_

namespace ee
{
	namespace Colour
	{
		XMGLOBALCONST XMVECTORF32 BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
		XMGLOBALCONST XMVECTORF32 WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
		XMGLOBALCONST XMVECTORF32 BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
		XMGLOBALCONST XMVECTORF32 RED = {1.0f, 0.0f, 0.0f, 1.0f};
		XMGLOBALCONST XMVECTORF32 GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
		XMGLOBALCONST XMVECTORF32 PURPLE = {1.0f, 0.0f, 1.0f, 1.0f};
		XMGLOBALCONST XMVECTORF32 YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
	}
}

#endif