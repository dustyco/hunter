

#include "Sim.h"
#include "PlayerDB.h"
#include "common_sfml_network.h"


bool Sim::Sim_tick (float dt)
{
	PlayerDB& player_db = PlayerDB::getSingleton();
	
	for (ShipMap::iterator ship_it=ships.begin(); ship_it!=ships.end(); ++ship_it)
	{
		Ship& ship = ship_it->second;
		
		// Get controls from the piloting player
		if (ship.pilot!=0 && player_db.has(ship.pilot) && player_db.get(ship.pilot).online)
			ship.pilot_controls = player_db.get(ship.pilot).pilot_controls;
		else
			ship.pilot_controls.clear();
		
		// Move the ship
		ship.applyControls(dt);
		ship.pos += ship.posv*dt;
		ship.rot += ship.rotv*dt;
		ship.posv *= 0.99f;
		ship.rotv *= 0.99f;
	}
	return true;
}


void Sim::getShipMovement (sf::Packet& packet)
{
	packet << uint32_t(ships.size());
	for (ShipMap::iterator ship_it=ships.begin(); ship_it!=ships.end(); ++ship_it)
	{
		ShipID id = ship_it->first;
		Ship& ship = ship_it->second;
		packet << id;
		packet << ship.pos << ship.posv;
		packet << ship.rot << ship.rotv;
	}
}

void Sim::setShipMovement (sf::Packet& packet)
{
	uint32_t count;
	if (!(packet >> count)) return;
	for (int i=0; i!=count; ++i)
	{
		ShipID id;
		if (!(packet >> id)) return;
		ShipMap::iterator ship_it;
		ship_it = ships.find(id);
		if (ship_it != ships.end())
		{
			Ship& ship = ship_it->second;
			packet >> ship.pos >> ship.posv;
			packet >> ship.rot >> ship.rotv;
		}
	}
	for (ShipMap::iterator ship_it=ships.begin(); ship_it!=ships.end(); ++ship_it)
	{
		ShipID id = ship_it->first;
		Ship& ship = ship_it->second;
		packet << id;
		packet << ship.pos << ship.posv;
		packet << ship.rot << ship.rotv;
	}
}
