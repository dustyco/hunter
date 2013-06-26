

#include "Ship.h"
#include <iostream>
using namespace std;


void Ship::clearPhysics () {
	pos = posv = Vec2::ORIGIN;
	rot = rotv = 0;
	mass = moment = 1;
}

void Ship::calculatePhysics () {

	// Find mass and center
	int n = 0;
	local_center = Vec2::ORIGIN;
	for (int y=0; y!=CELL_STRIDE; ++y)
	for (int x=0; x!=CELL_STRIDE; ++x) {
		if (cells[x][y].there) {
			local_center += Vec2(x, y);
			++n;
		}
	}
	if (n==0) return;
	local_center /= float(n);
	local_center *= CELL_WIDTH;
	mass = CELL_MASS*n;
//	cout << "Mass: " << mass << endl;
	
	// Find moment of inertia
	// http://en.wikipedia.org/wiki/Parallel_axis_theorem
	float term_1 = CELL_MOMENT*n;
	float term_2 = 0;
	for (int y=0; y!=CELL_STRIDE; ++y)
	for (int x=0; x!=CELL_STRIDE; ++x) {
		if (cells[x][y].there) {
			Vec2 diff = Vec2(x, y)*CELL_WIDTH - local_center;
			diff *= diff;
			term_2 += diff.x + diff.y;  // r^2
		}
	}
	term_2 *= CELL_MASS;
	moment = term_1 + term_2;
//	cout << "Moment: " << moment << endl;
}

void Ship::applyControls (float dt) {
	float thrust = 100000;
	posv += Mat2(rot)*pilot_controls.translate*thrust/mass*dt;
	rotv += pilot_controls.rotate*thrust/moment*dt;
}

