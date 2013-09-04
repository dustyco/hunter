

#include <iostream>
#include <algorithm>
#include "common.h"
#include "ClientApp.h"
using namespace std;


int ClientApp::go (int argc, char const** argv)
{
	try
	{
		args.resize(argc);
		copy(argv, argv+argc, args.begin());
		
		setup();
		while (loop()) {}
		cleanup();
	}
	catch (exception& e)
	{
		if (!string(e.what()).empty())
			cout << "Unhandled exception: " << e.what() << endl;
			
		return 1;
	}
	
	return 0;
}

bool ClientApp::setup ()
{
	// Server hostname
	if (args.size() > 1) net.server_hostname = args[1];
	else
	{
		cout << "Usage: " << args[0] << " [hostname]" << endl;
		cout << "\tConnects to [hostname]:" << net::DEFAULT_PORT << endl;
		throw runtime_error("");
	}
	
	renderer.init();
	
	{
		Ship ship;
		for (int y=0; y!=16; ++y){
			for (int x=0; x!=5; ++x) {
				ship.cells[x][y].there = true;
			}
		}
		ship.calculatePhysics();
		sim.ships[0] = ship;
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
		sim.ships[1] = ship;
	}
	
	net.pilot_controls.clear();
	
	net.ClientNet_init();
	
	return true;
}

bool ClientApp::loop ()
{
	float dt = 1.0/60.0;
	
	// INPUT ////////////////////////////////////////////////////////////////////////////
	handleInput();
	if (net.status == ClientNet::QUIT) return false;
	
	// SIMULATE /////////////////////////////////////////////////////////////////////////
	net.ClientNet_tick(dt);
	
	// Apply ship movement updates from ClientNet
	while (!net.ship_movement_packets.empty())
	{
		sim.setShipMovement(net.ship_movement_packets.front());
		net.ship_movement_packets.pop_front();
	}
	
	sim.Sim_tick(dt);
	
	renderer.render(sim);
	
	return true;
}

bool ClientApp::cleanup ()
{
	return true;
}

void ClientApp::handleInput ()
{
	// TODO Copy code from glide that only grabs states when window is focused
	// State based controls
//	grab = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	mouse_screen = renderer.getMouseScreen();
	mouse_world = renderer.getMouseWorld();
	
	net.pilot_controls.translate.y = 
		  (sf::Keyboard::isKeyPressed(sf::Keyboard::W)?(1):(0))
		- (sf::Keyboard::isKeyPressed(sf::Keyboard::S)?(1):(0));
	net.pilot_controls.translate.x = 
		  (sf::Keyboard::isKeyPressed(sf::Keyboard::E)?(1):(0))
		- (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)?(1):(0));
	net.pilot_controls.rotate =
		  (sf::Keyboard::isKeyPressed(sf::Keyboard::A)?(1):(0))
		- (sf::Keyboard::isKeyPressed(sf::Keyboard::D)?(1):(0));
	
	// Event based controls
	sf::Event event;
	while ( renderer.getEvent(event) ) {
		switch (event.type) {
			case sf::Event::Closed:             net.disconnect("Window closed"); return;
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
					case sf::Keyboard::Escape:  net.disconnect("Escape key pressed"); return;
				}
				break;
		}
	}
}
