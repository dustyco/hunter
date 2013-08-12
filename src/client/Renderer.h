#pragma once

#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>
#ifdef _WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <SFML/Graphics.hpp>
#include "common.h"

#include <sstream>
#include "util/convert_sf_vector.h"

#include "Sim.h"
const float PI = 3.14159265;
const float DEG_PER_RAD = 180.0/PI;
const int CURRENT_SHIP = 1;

class Renderer{
	public:
		Renderer();
		~Renderer();
	
		bool init();
		
		void render(Sim& data);
		
		bool cleanup();
		
		sf::Vector2i getMouseScreen();
		sf::Vector2f getMouseWorld();
		
		bool getEvent(sf::Event& evt);
		
		void zoom(float factor);
		
	private:
		sf::RenderWindow window;
		void ClientSim_draw (sf::RenderTarget& target, Sim& data);
		sf::IntRect partTexture (PartID id, int size);
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
};
