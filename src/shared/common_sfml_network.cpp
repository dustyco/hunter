

#include "common_sfml_network.h"


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


