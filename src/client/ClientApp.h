#pragma once

#include "Renderer.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <sstream>
#include <iomanip>
#ifdef _WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include "common.h"
#include "Sim.h"
#include "ClientNet.h"
#include "util/convert_sf_vector.h"


struct ClientApp
{
	Sim sim;
	ClientNet network;	
	
	sf::Clock        clock;
	sf::Vector2i     mouse_screen;
	sf::Vector2f     mouse_world;
	
	int  go          (int argc, char const** argv);
	bool setup       (int argc, char const** argv);
	bool loop        ();
	bool cleanup     ();
	void handleInput ();
	
	//^old App --- v New Stuff
	
	Renderer renderer;
	
	// v old Sim
	
	
	bool ClientSim_init ();
	void ClientSim_tick (float dt);
};

