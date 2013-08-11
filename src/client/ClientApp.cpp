#include "ClientApp.h"


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

using namespace std;



bool ClientApp::ClientSim_init ()
{
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
	
	if (hull_texture.loadFromFile("block-16.png")) {
		hull_texture.setSmooth(true);
		hull_sprite.setTexture(hull_texture);
		sf::Vector2<unsigned int> size = hull_texture.getSize();
		hull_sprite.setOrigin(size.x/2, size.y/2);
		float scale = 2.0/size.x;
		hull_sprite.setScale(scale, scale);
	} else return false;
	
/*	if (part_texture.loadFromFile("parts.png")) {
		part_texture.setSmooth(true);
		part_sprite.setTexture(part_texture);
		part_sprite.setOrigin(32, 32);
		part_sprite.setScale(2.0/64, -2.0/64);
	} else return false;
*/	
		{
			Ship ship;
			for (int y=0; y!=16; ++y)
			for (int x=0; x!=5; ++x) {
				ship.cells[x][y].there = true;
			}
			ship.calculatePhysics();
			ships[0] = ship;
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
			ships[1] = ship;
		}
	
	pilot_controls.clear();
	
	ClientNet_init();
	
	cam_rot = 0;
	cam_pos = Vec2::ORIGIN;
	
	return true;
}

void ClientApp::ClientSim_tick (float dt)
{
	ClientNet_tick(dt);
	
	// Apply ship movement updates from ClientNet
	while (!ship_movement_packets.empty())
	{
		setShipMovement(ship_movement_packets.front());
		ship_movement_packets.pop_front();
	}
	
	// Simulate
	Sim_tick(dt);
}

void ClientApp::ClientSim_draw (sf::RenderTarget& target)
{
	// WORLD VIEW //////////////////////////////////////////////////////////////////////
	cam_pos = ships[CURRENT_SHIP].pos;
//	cam_rot = ships[CURRENT_SHIP].rot;
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
	for (ShipMap::const_iterator ship_it=ships.begin(); ship_it!=ships.end(); ++ship_it) {
		const Ship& ship = ship_it->second;
//		part_sprite.setRotation(ship.rot*DEG_PER_RAD);
		hull_sprite.setRotation(ship.rot*DEG_PER_RAD);
		hull_sprite.setColor(sf::Color(40, 240, 40, 255));
		
		// Draw each cell
		Mat2 rot(ship.rot);
		for (int y=0; y!=CELL_STRIDE; ++y)
		for (int x=0; x!=CELL_STRIDE; ++x) {
			const Cell& cell = ship.cells[x][y];
			if (cell.there) {
				Vec2 local = Vec2(x, y)*CELL_WIDTH - ship.local_center;
				Vec2 global = rot*local + ship.pos;
				
				hull_sprite.setPosition(global.x, global.y);
				target.draw(hull_sprite);
				
/*				if (cell.part) {
					part_sprite.setPosition(global.x, global.y);
					part_sprite.setTextureRect(partTexture(cell.part->id, 64));
					target.draw(part_sprite);
				}
*/			}
		}
	}
}

sf::IntRect ClientApp::partTexture (PartID id, int size)
{
	PartID row = id / SHIP_PART_STRIDE;
	PartID col = id % SHIP_PART_STRIDE;
//	cout << id << ", " << col << ", " << row << endl;
	return sf::IntRect(col*size, row*size, size, size);
}

// The program starts and ends here
int ClientApp::go (int argc, char const** argv) {
	if (!setup(argc, argv)) {
		cout << "Error: ClientApp::setup() returned false" << endl;
		return 1;
	}
	while (loop()) {}
	if (!cleanup()) {
		cout << "Error: ClientApp::cleanup() returned false" << endl;
		return 1;
	}
	return 0;
}

bool ClientApp::setup (int argc, char const** argv) {
	
	// Server hostname
	if (argc>1) {
		server_hostname = argv[1];
	} else {
		cout << "Usage: " << argv[0] << " [hostname]" << endl;
		cout << "\tConnects to [hostname]:" << net::DEFAULT_PORT << endl;
		return false;
	}
	
	// SET UP WINDOW ///////////////////////////////////////////////////////////////////
	string window_title = "Hunter";
	sf::VideoMode video_mode = sf::VideoMode::getDesktopMode();
	#ifdef SFML_SYSTEM_WINDOWS
		// Windows will literally make the window this size and
		// let it get covered by the task bar so shrink it
		// Also, when the user unmaximizes it, this is what
		// will be remembered
		video_mode.width -= 10;
		video_mode.height -= 100;
	#endif
	window.create( video_mode, window_title, sf::Style::Resize | sf::Style::Close );
	#ifdef SFML_SYSTEM_WINDOWS
		// Maximize to fill the screen
		ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);
	#endif
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(true);
	
	cam_zoom = 20;
	
	ClientSim_init();
	
	return true;
}

bool ClientApp::loop () {
	
	// INPUT ////////////////////////////////////////////////////////////////////////////
	handleInput();
	if (status == QUIT) return false;
	
	// SIMULATE /////////////////////////////////////////////////////////////////////////
	ClientSim_tick(1.0/60.0);
	
	// DRAW WORLD ////////////////////////////////////////////////////////////////////////
	window.clear();
	ClientSim_draw(window);
	window.display();
	
	return true;
}

bool ClientApp::cleanup () {
	return true;
}

void ClientApp::handleInput ()
{
	// State based controls
//	grab = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	mouse_screen = sf::Mouse::getPosition(window);
	mouse_world = window.mapPixelToCoords(mouse_screen);
	
	pilot_controls.translate.y = 
		  (sf::Keyboard::isKeyPressed(sf::Keyboard::W)?(1):(0))
		- (sf::Keyboard::isKeyPressed(sf::Keyboard::S)?(1):(0));
	pilot_controls.translate.x = 
		  (sf::Keyboard::isKeyPressed(sf::Keyboard::E)?(1):(0))
		- (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)?(1):(0));
	pilot_controls.rotate =
		  (sf::Keyboard::isKeyPressed(sf::Keyboard::A)?(1):(0))
		- (sf::Keyboard::isKeyPressed(sf::Keyboard::D)?(1):(0));
	
	// Event based controls
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:             disconnect("Window closed"); return;
//			case sf::Event::LostFocus:          releaseControls(); break;
//			case sf::Event::GainedFocus:        break;
//			case sf::Event::Resized:            break;
//			case sf::Event::MouseMoved:         event.mouseMove.x; break;
			case sf::Event::MouseButtonPressed:
//				switch (event.mouseButton.button) {
//					case sf::Mouse::Left:       mouseLeft(); break;
//					case sf::Mouse::Right:      mouseRight(); break;
//				}
				break;
			case sf::Event::MouseWheelMoved:
				cam_zoom *= pow(0.9, event.mouseWheel.delta);
				cam_zoom = min(max(cam_zoom, 10.0f), 100.0f);
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Escape:  disconnect("Escape key pressed"); return;
				}
				break;
		}
	}
}
