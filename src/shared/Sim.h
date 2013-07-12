

#pragma once
#include <map>
#include <SFML/Network.hpp>
#include "common.h"
#include "Ship.h"
typedef std::map<ShipID,Ship> ShipMap;


struct Sim
{
	ShipMap ships;
	ShipID  next_ship_id;
	
	     Sim             () : next_ship_id(1) {}
	bool Sim_tick        (float dt);
	void getShipMovement (sf::Packet& packet);
	void setShipMovement (sf::Packet& packet);
};
