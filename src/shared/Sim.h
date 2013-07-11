

#pragma once
#include <vector>
#include "common.h"
#include "Ship.h"
typedef std::vector<Ship> ShipVector;


struct Sim {
	ShipVector ships;
	
	bool Sim_tick (float dt);
};
