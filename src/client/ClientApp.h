#pragma once


#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>
#include "common.h"
#include "Sim.h"
#include "ClientNet.h"
#include "Renderer.h"
#include "util/convert_sf_vector.h"
using namespace std;


struct ClientApp
{
	Sim            sim;
	ClientNet      net;
	Renderer       renderer;
	vector<string> args;
	sf::Clock      clock;
	sf::Vector2i   mouse_screen;
	sf::Vector2f   mouse_world;
	
	int  go          (int argc, char const** argv);
	bool setup       ();
	bool loop        ();
	bool cleanup     ();
	void handleInput ();
};

