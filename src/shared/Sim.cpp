

#include "Sim.h"


void Sim::Sim_tick (float dt) {
	for (ShipVector::iterator ship_it=ships.begin(); ship_it!=ships.end(); ++ship_it) {
		Ship& ship = *ship_it;
		
		ship.applyControls(dt);
		ship.pos += ship.posv*dt;
		ship.rot += ship.rotv*dt;
		ship.posv *= 0.99f;
		ship.rotv *= 0.99f;
	}
}
