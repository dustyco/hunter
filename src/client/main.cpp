

/*
	Main loop, input, drawing, interface with SFML
*/

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
#include "ClientSim.h"
using namespace std;


struct ClientApp : ClientSim {
	bool             running;
	sf::RenderWindow window;
	sf::Clock        clock;
	sf::Vector2i     mouse_screen;
	sf::Vector2f     mouse_world;
	
	int  go          (int argc, char const** argv);
	bool setup       (int argc, char const** argv);
	bool loop        ();
	bool cleanup     ();
	void handleInput ();
};

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
		return 1;
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
	
	// True until we're ready to exit
	running = true;
	
	cam_zoom = 20;
	
	ClientSim_init();
	
	return true;
}

bool ClientApp::loop () {
	
	// INPUT ////////////////////////////////////////////////////////////////////////////
	handleInput();
	if (!running) return false;

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
	
	player_flight_controls.forward = sf::Keyboard::isKeyPressed(sf::Keyboard::W)?(1):(0);
	player_flight_controls.backward = sf::Keyboard::isKeyPressed(sf::Keyboard::S)?(1):(0);
	player_flight_controls.turn_left = sf::Keyboard::isKeyPressed(sf::Keyboard::A)?(1):(0);
	player_flight_controls.turn_right = sf::Keyboard::isKeyPressed(sf::Keyboard::D)?(1):(0);
	
	// Event based controls
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:             running = false; return;
//			case sf::Event::LostFocus:          releaseControls(); break;
//			case sf::Event::GainedFocus:        break;
//			case sf::Event::Resized:            break;
//			case sf::Event::MouseMoved:         event.mouseMove.x; break;
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button) {
//					case sf::Mouse::Left:       mouseLeft(); break;
//					case sf::Mouse::Right:      mouseRight(); break;
				}
				break;
			case sf::Event::MouseWheelMoved:
				cam_zoom *= pow(0.9, event.mouseWheel.delta);
				cam_zoom = min(max(cam_zoom, 10.0f), 100.0f);
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Escape:  running = false; return;
				}
				break;
		}
	}
}

#ifdef _MSC_VER
	#ifdef DEBUG
		#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
	#else
		#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	#endif
#endif

int main (int argc, char const** argv) {
	ClientApp app;
	return app.go(argc, argv);
}
