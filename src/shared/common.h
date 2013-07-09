

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


class NonCopyable
{
protected:
	NonCopyable () {}
	~NonCopyable () {}  // Protected non-virtual destructor
private: 
	NonCopyable (const NonCopyable &);
	NonCopyable & operator = (const NonCopyable &);
};


typedef uint32_t PlayerID;


// Analog axes from -1 to 1
struct PilotControls
{
	Vec2 translate;
	float rotate;
	PilotControls () { clear(); }
	void clear () { translate = Vec2::ORIGIN; rotate = 0; }
};


