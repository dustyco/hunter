

#include <vector>
#include "common.h"
#include "Cell.h"


struct Ship {
	// Cells
	Cell cells[CELL_STRIDE][CELL_STRIDE];
	
	// Physics
	Vec2 pos, posv;
	float rot, rotv;
	float mass, moment;
	Vec2 local_center;
	
	// Flight controls (analog 0 to 1)
	struct FlightControls {
		float forward, backward, turn_left, turn_right;
		FlightControls () { clear(); }
		void clear () { forward = backward = turn_left = turn_right = 0; }
	} flight_controls;
	
	Ship () { clearPhysics(); }
	void clearPhysics ();
	void calculatePhysics ();
	void applyControls (float dt);
	
	Ship (const Ship& ship) { *this = ship; }
};
typedef std::vector<Ship> ShipVector;
