

#pragma once
#ifndef _WIN32
	#include <csignal>
#endif
#include <iostream>
#include <SFML/Network.hpp>
#include "Sim.h"
#include "ServerNet.h"
using namespace std;


struct ServerSim : public Sim, public ServerNet
{
	
	bool ServerSim_init ()
	{
/*		{
			Ship ship;
			for (int y=0; y!=16; ++y)
			for (int x=0; x!=5; ++x) {
				ship.cells[x][y].there = true;
			}
			ship.calculatePhysics();
			ships.push_back(ship);
		}
		{
			Ship ship;
			ship.cells[1][0].there = true;
			ship.cells[0][1].there = true;
			ship.cells[1][1].there = true;
			ship.cells[2][1].there = true;
			ship.cells[0][2].there = true;
			ship.cells[1][2].there = true;
			ship.cells[2][2].there = true;
			ship.cells[1][3].there = true;
			ship.cells[1][4].there = true;
			
//			ship.cells[1][2].part = new Part(TRACTOR_BEAM);
			ship.calculatePhysics();
			ship.posv = Vec2(1, 0);
			ship.rotv = 0;
			ships.push_back(ship);
		}
*/		
		if (!ServerNet_init()) return false;
		
		return true;
	}
	
	void ServerSim_cleanup ()
	{
		ServerNet_cleanup();
	}
	
	bool ServerSim_tick (float dt)
	{
		if (!ServerNet_tick(dt)) return false;
		if (!Sim_tick(dt)) return false;
		
		// Get latest ship movement and put it in ServerNet to be broadcast
		ship_movement.clear();
		ship_movement << net::MSG_TYPE_SHIP_MOVEMENT;
		getShipMovement(ship_movement);
	
		return true;
	}
	
};


