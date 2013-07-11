

#include "Sim.h"
#include "PlayerDB.h"


bool Sim::Sim_tick (float dt)
{
	PlayerDB& player_db = PlayerDB::getSingleton();
	
	for (ShipVector::iterator ship_it=ships.begin(); ship_it!=ships.end(); ++ship_it) {
		Ship& ship = *ship_it;
		
		// Get controls from the piloting player
		if (player_db.has(ship.pilot) && player_db.get(ship.pilot).online)
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
