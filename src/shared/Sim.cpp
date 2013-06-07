

#include "Sim.h"

#include <iostream>
using namespace std;

void Sim::Ship::clearCells () {
	for (int y=0; y!=STRIDE; ++y)
	for (int x=0; x!=STRIDE; ++x) {
		cells[x][y] = false;
	}
}

void Sim::Ship::clearPhysics () {
	pos = posv = Vec2::ORIGIN;
	rot = rotv = 0;
	mass = moment = 1;
}

void Sim::Ship::calculatePhysics () {

	// Find mass and center
	int n = 0;
	local_center = Vec2::ORIGIN;
	for (int y=0; y!=STRIDE; ++y)
	for (int x=0; x!=STRIDE; ++x) {
		if (cells[x][y]) {
			local_center += Vec2(x, y);
			++n;
		}
	}
	if (n==0) return;
	local_center /= float(n);
	local_center *= CELL_WIDTH;
	mass = CELL_MASS*n;
	cout << "Mass: " << mass << endl;
	
	// Find moment of inertia
	// http://en.wikipedia.org/wiki/Parallel_axis_theorem
	float term_1 = CELL_MOMENT*n;
	float term_2 = 0;
	for (int y=0; y!=STRIDE; ++y)
	for (int x=0; x!=STRIDE; ++x) {
		if (cells[x][y]) {
			Vec2 diff = Vec2(x, y)*CELL_WIDTH - local_center;
			diff *= diff;
			term_2 += diff.x + diff.y;  // r^2
		}
	}
	term_2 *= CELL_MASS;
	moment = term_1 + term_2;
	cout << "Moment: " << moment << endl;
}

void Sim::Ship::applyControls (float dt) {
	if (flight_controls.forward>0 || flight_controls.backward>0 || flight_controls.turn_left>0 || flight_controls.turn_right>0) {
		float thrust = 100000;
		Vec2 front = Mat2(rot)*Vec2::Y_UNIT;
		posv += front*((flight_controls.forward-flight_controls.backward)*dt/mass*thrust);
		rotv += (flight_controls.turn_left-flight_controls.turn_right)*dt/moment*thrust/2;
	}
}

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
