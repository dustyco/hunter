

#pragma once
#include <stdint.h>
#include "hmath/all.h"
using namespace hmath;
typedef vec<2,int>    Vec2i;
typedef vec<3,int>    Vec3i;
typedef vec<4,int>    Vec4i;
typedef vec<2,float>  Vec2;
typedef vec<3,float>  Vec3;
typedef vec<4,float>  Vec4;
typedef mat<2,float>  Mat2;
typedef mat<3,float>  Mat3;
typedef mat<4,float>  Mat4;
typedef line<2,float> Line2;
typedef line<3,float> Line3;
typedef line<4,float> Line4;


typedef uint32_t PlayerID;


class NonCopyable
{
protected:
	NonCopyable () {}
	~NonCopyable () {}  // Protected non-virtual destructor
private: 
	NonCopyable (const NonCopyable &);
	NonCopyable & operator = (const NonCopyable &);
};

namespace net
{
	const uint16_t DEFAULT_PORT = 32123;
	const uint32_t GREET_NUMBER = 1452267901;
	const uint32_t GREET_VERSION = 0;
	const uint32_t GREET_NAME_TYPE_NEW = 0;
	const uint32_t GREET_NAME_TYPE_EXISTING = 1;
};

