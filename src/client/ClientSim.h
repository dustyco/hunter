

#include <SFML/Graphics.hpp>
#include "Sim.h"
#include "util/convert_sf_vector.h"

#include <iostream>
using namespace std;

const float PI = 3.14159265;
const float DEG_PER_RAD = 180.0/PI;
const int CURRENT_SHIP = 1;

struct ClientSim : public Sim {
	sf::Texture stars_far;
	sf::Texture stars_medium;
	sf::Texture stars_close;
	sf::Texture block_texture;
	sf::Sprite  block_sprite;
	Vec2        cam_pos;  // Meters
	float       cam_rot;  // Radians
	float       cam_zoom; // Meters from top to bottom of screen
	Ship::FlightControls player_flight_controls;
	
	bool ClientSim_init () {
		if (stars_far.loadFromFile("stars_far.jpg")) {
			stars_far.setSmooth(true);
			stars_far.setRepeated(true);
		}
		if (stars_medium.loadFromFile("stars_medium.jpg")) {
			stars_medium.setSmooth(true);
			stars_medium.setRepeated(true);
		}
		if (stars_close.loadFromFile("stars_close.jpg")) {
			stars_close.setSmooth(true);
			stars_close.setRepeated(true);
		}
		
		if (block_texture.loadFromFile("block-16.png")) {
			block_texture.setSmooth(true);
			block_sprite.setTexture(block_texture);
	//		block_sprite.setTextureRect(sf::IntRect(64*4, 64*5, 64, 64));
			sf::Vector2<unsigned int> size = block_texture.getSize();
			block_sprite.setOrigin(size.x/2, size.y/2);
			float scale = 2.0/size.x;
			block_sprite.setScale(scale, scale);
		} else return false;
		
		{
			Ship ship;
			for (int y=0; y!=16; ++y)
			for (int x=0; x!=5; ++x) {
				ship.cells[x][y] = true;
			}
			ship.calculatePhysics();
			ships.push_back(ship);
		}
		{
			Ship ship;
			ship.cells[1][0] = true;
			ship.cells[0][1] = true;
			ship.cells[1][1] = true;
			ship.cells[2][1] = true;
			ship.cells[0][2] = true;
			ship.cells[1][2] = true;
			ship.cells[2][2] = true;
			ship.cells[1][3] = true;
			ship.cells[1][4] = true;
			ship.calculatePhysics();
			ship.posv = Vec2(1, 0);
			ship.rotv = 0;
			ship.flight_controls.turn_right = 0.3;
			ship.flight_controls.forward = 0.5;
			ships.push_back(ship);
		}
		
		player_flight_controls.clear();
		
		return true;
	}
	
	void ClientSim_tick (float dt) {
		ships[CURRENT_SHIP].flight_controls = player_flight_controls;
		Sim_tick(dt);
	}
	
	void ClientSim_draw (sf::RenderTarget& target) {
		
		// WORLD VIEW //////////////////////////////////////////////////////////////////////
		cam_pos = ships[CURRENT_SHIP].pos;
//		cam_rot = ships[CURRENT_SHIP].rot;
		int w = target.getSize().x;
		int h = target.getSize().y;
		float screen_aspect = float(w)/float(h);
		sf::View view;
		view.setCenter(cam_pos.x, cam_pos.y);
		view.setRotation(cam_rot*DEG_PER_RAD);
		view.setSize(screen_aspect*cam_zoom, -cam_zoom);
		target.setView(view);
		
		// DRAW BACKGROUND //////////////////////////////////////////////////////////////////
		{
			sf::RenderStates state(sf::BlendAdd);
			sf::View unit_view(sf::FloatRect(0, 1, 1, -1));
			sf::VertexArray vert(sf::Quads, 4);
			vert[0].position = sf::Vector2f(0, 0);
			vert[1].position = sf::Vector2f(1, 0);
			vert[2].position = sf::Vector2f(1, 1);
			vert[3].position = sf::Vector2f(0, 1);
			
			// Far
			Vec2 c = convert(view.getCenter());
			c *= 1.0f;
			float factor = 100.0/120;
			vert[0].texCoords = sf::Vector2f(c.x-w/2*factor, c.y-h/2*factor);
			vert[1].texCoords = sf::Vector2f(c.x+w/2*factor, c.y-h/2*factor);
			vert[2].texCoords = sf::Vector2f(c.x+w/2*factor, c.y+h/2*factor);
			vert[3].texCoords = sf::Vector2f(c.x-w/2*factor, c.y+h/2*factor);
			
			state.texture = &stars_far;
			target.setView(unit_view);
			target.draw(vert, state);
			
			// Medium
			c = convert(view.getCenter());
			c *= 2.0f;
			factor = 100.0/120;
			vert[0].texCoords = sf::Vector2f(c.x-w/2*factor, c.y-h/2*factor);
			vert[1].texCoords = sf::Vector2f(c.x+w/2*factor, c.y-h/2*factor);
			vert[2].texCoords = sf::Vector2f(c.x+w/2*factor, c.y+h/2*factor);
			vert[3].texCoords = sf::Vector2f(c.x-w/2*factor, c.y+h/2*factor);
			
			state.texture = &stars_medium;
			target.setView(unit_view);
			target.draw(vert, state);
			
			// Close
			c = convert(view.getCenter());
			c *= 4.0f;
			factor = 100.0/120;
			vert[0].texCoords = sf::Vector2f(c.x-w/2*factor, c.y-h/2*factor);
			vert[1].texCoords = sf::Vector2f(c.x+w/2*factor, c.y-h/2*factor);
			vert[2].texCoords = sf::Vector2f(c.x+w/2*factor, c.y+h/2*factor);
			vert[3].texCoords = sf::Vector2f(c.x-w/2*factor, c.y+h/2*factor);
			
			state.texture = &stars_close;
			target.setView(unit_view);
			target.draw(vert, state);
		}
		target.setView(view);
		
		// DRAW EACH SHIP //////////////////////////////////////////////////////////////////
		for (ShipVector::const_iterator ship_it=ships.begin(); ship_it!=ships.end(); ++ship_it) {
			const Ship& ship = *ship_it;
			block_sprite.setRotation(ship.rot*DEG_PER_RAD);
			block_sprite.setColor(sf::Color(40, 240, 40, 255));
			
			// Draw each cell
			Mat2 rot(ship.rot);
			for (int y=0; y!=STRIDE; ++y)
			for (int x=0; x!=STRIDE; ++x) {
				if (ship.cells[x][y]) {
					Vec2 local = Vec2(x, y)*CELL_WIDTH - ship.local_center;
					Vec2 global = rot*local + ship.pos;
					
					block_sprite.setPosition(global.x, global.y);
					target.draw(block_sprite);
				}
			}
		}
	}
};


