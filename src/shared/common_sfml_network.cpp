

#include "common_sfml_network.h"


sf::Packet& operator << (sf::Packet& packet, const PilotControls& c)
{
	// TODO Compress (this code isn't working right)
/*	typedef int8_t axis_type;
	return packet << compressVec<3,axis_type>(Vec3(c.translate.x, c.translate.y, c.rotate));
*/
	return packet << c.translate << c.rotate;
}

sf::Packet& operator >> (sf::Packet& packet, PilotControls& c)
{
	// TODO Compress (this code isn't working right)
/*	typedef int8_t axis_type;
	vec<3,axis_type> vi;
	packet >> vi;
	vec<3,float> vf = decompressVec<3,axis_type>(vi);
	c.translate = Vec2(vf.x, vf.y);
	c.rotate = vf.z;
	return packet;
*/
	return packet >> c.translate >> c.rotate;
}


