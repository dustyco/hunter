

#pragma once
#include <vector>
#include "common.h"
#include "Cell.h"


struct Ship {
	// Cells
	Cell cells[CELL_STRIDE][CELL_STRIDE];
	
	// Physics
	Vec2          pos, posv;
	float         rot, rotv;
	float         mass, moment;
	Vec2          local_center;
	
	// Controls
	PlayerID      pilot;
	PilotControls pilot_controls;
	
	Ship () { clearPhysics(); }
	void clearPhysics ();
	void calculatePhysics ();
	void applyControls (float dt);
	
	Ship (const Ship& ship) { *this = ship; }
};
typedef std::vector<Ship> ShipVector;
