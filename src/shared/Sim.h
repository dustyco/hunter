

#pragma once
#include <vector>
#include "common.h"
#include "Ship.h"


struct Sim {
	ShipVector ships;
	
	void Sim_tick (float dt);
};
