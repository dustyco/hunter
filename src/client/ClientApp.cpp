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
	{
		Ship ship;
		for (int y=0; y!=16; ++y){
			for (int x=0; x!=5; ++x) {
				ship.cells[x][y].there = true;
			}
		}
		ship.calculatePhysics();
		ships[0] = ship;
	}
	{
		Ship ship;
		//ship.cells[1][0].there = true;//float
		ship.cells[0][1].there = true;
		ship.cells[1][1].there = true;
		ship.cells[2][1].there = true;
		//ship.cells[0][2].there = true;//ttp
		ship.cells[1][2].there = true;
		//ship.cells[2][2].there = true;// = 0
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

// The program starts and ends here
int ClientApp::go (int argc, char const** argv) {
	if (!setup(argc, argv)) {
		cout << "Error: ClientApp::setup() returned false" << endl;
		return 1;
	}
	if (!renderer.init()) {
		cout << "Error: Renderer::init() returned false" << endl;
		return 1;
	}
	while (loop()) {}
	if (!cleanup()) {
		cout << "Error: ClientApp::cleanup() returned false" << endl;
		return 2;
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
	
	ClientSim_init();
	
	return true;
}

bool ClientApp::loop () {
	
	// INPUT ////////////////////////////////////////////////////////////////////////////
	handleInput();
	if (status == QUIT) return false;
	
	// SIMULATE /////////////////////////////////////////////////////////////////////////
	ClientSim_tick(1.0/60.0);
	
	renderer.render((*this));
	
	return true;
}

bool ClientApp::cleanup () {
	return true;
}

void ClientApp::handleInput ()
{
	// State based controls
//	grab = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	mouse_screen = renderer.getMouseScreen();
	mouse_world = renderer.getMouseWorld();
	
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
	while ( renderer.getEvent(event) ) {
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
				renderer.zoom(pow(0.9, event.mouseWheel.delta));
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Escape:  disconnect("Escape key pressed"); return;
				}
				break;
		}
	}
}
