

#include <vector>
#include "common.h"


const int   STRIDE = 20;
const float CELL_WIDTH = 2;
const float CELL_MASS = 1000;
const float CELL_MOMENT = CELL_MASS*(CELL_WIDTH*CELL_WIDTH)/6;  // http://en.wikipedia.org/wiki/List_of_moments_of_inertia

struct Sim {
	
	struct Ship {
		// Cells
		bool cells[STRIDE][STRIDE];
		
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
		
		Ship () { clearCells(); clearPhysics(); }
		void clearCells ();
		void clearPhysics ();
		
		void calculatePhysics ();
		void applyControls (float dt);
	};
	typedef std::vector<Ship> ShipVector;
	
	ShipVector ships;
	
	void Sim_tick (float dt);
};
