#pragma once

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


const float PI = 3.14159265;
const float DEG_PER_RAD = 180.0/PI;
const int CURRENT_SHIP = 1;

struct ClientApp : public Sim, public ClientNet
{
	sf::RenderWindow window;
	sf::Clock        clock;
	sf::Vector2i     mouse_screen;
	sf::Vector2f     mouse_world;
	
	int  go          (int argc, char const** argv);
	bool setup       (int argc, char const** argv);
	bool loop        ();
	bool cleanup     ();
	void handleInput ();
	
	//^old App --- v old Sim
	
	
	sf::Texture   stars_far;
	sf::Texture   stars_medium;
	sf::Texture   stars_close;
	sf::Texture   part_texture;
	sf::Sprite    part_sprite;
	sf::Texture   hull_texture;
	sf::Sprite    hull_sprite;
	Vec2          cam_pos;  // Meters
	float         cam_rot;  // Radians
	float         cam_zoom; // Meters from top to bottom of screen
	
	bool ClientSim_init ();
	void ClientSim_tick (float dt);
	void ClientSim_draw (sf::RenderTarget& target);
	sf::IntRect partTexture (PartID id, int size);
};

