

#pragma once
#include <limits>
#include <SFML/Network.hpp>
#include "common.h"


template <class Integer>
Integer compressFloat (const float& f)
{
	return Integer(f*float(std::numeric_limits<Integer>::max()));
}
template <class Integer>
float decompressFloat (const Integer& i)
{
	return float(i)/float(std::numeric_limits<Integer>::max());
}

// Quantize floats into integer ranges (0 to 1 or -1 to 1 depending on un/signed)
template <int N, class Integer>
vec<N,Integer> compressVec (const vec<N,float>& vf) {
	vec<N,Integer> vi;
	for (int i=0; i!=N; ++i) vi[i] = Integer(vf[i] * float(std::numeric_limits<Integer>::max()));
	return vi;
}
template <int N, class Integer>
vec<N,float> decompressVec (const vec<N,Integer>& vi) {
	vec<N,float> vf;
	for (int i=0; i!=N; ++i) vf[i] = float(vf[i]) / float(std::numeric_limits<Integer>::max());
	return vf;
}

// Send a vector
template <int N, class R>
sf::Packet& operator << (sf::Packet& packet, const vec<N,R>& v) {
	for (int i=0; i!=N; ++i) packet << v[i];
	return packet;
}
template <int N, class R>
sf::Packet& operator >> (sf::Packet& packet, vec<N,R>& v) {
	for (int i=0; i!=N; ++i) packet >> v[i];
	return packet;
}

sf::Packet& operator << (sf::Packet& packet, const PilotControls& c)
{
	typedef int8_t axis_type;
	return packet << compressVec<3,axis_type>(Vec3(c.translate.x, c.translate.y, c.rotate));
}

sf::Packet& operator >> (sf::Packet& packet, PilotControls& c)
{
	typedef int8_t axis_type;
	vec<3,axis_type> vi;
	packet >> vi;
	vec<3,float> vf = decompressVec<3,axis_type>(vi);
	c.translate = Vec2(vf.x, vf.y);
	c.rotate = vf.z;
	return packet;
}


