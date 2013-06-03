

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
#include <SFML/System/Clock.hpp>
#include "common.h"
using namespace std;


struct ClientApp {
	bool             running;
	sf::RenderWindow window;
	sf::Clock        clock;
	sf::Texture      block_texture;
	sf::Sprite       block_sprite;
	sf::Vector2i     mouse_screen;
	sf::Vector2f     mouse_world;
	float            zoom;
	
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
	
	// SET UP WINDOW ///////////////////////////////////////////////////////////////////
	string window_title = "Hunter";
	sf::VideoMode video_mode = sf::VideoMode::getDesktopMode();
//	window.create( video_mode, window_title, sf::Style::Fullscreen | sf::Style::Close );
	window.create( video_mode, window_title, sf::Style::Resize | sf::Style::Close );
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(true);
	#ifdef SFML_SYSTEM_WINDOWS
		// Maximize on Windows because it can't figure out how to display properly
		ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);
	#endif
	
	// True until we're ready to exit
	running = true;
	
	if (block_texture.loadFromFile("block-16.png")) {
		block_texture.setSmooth(true);
		block_sprite.setTexture(block_texture);
//		block_sprite.setTextureRect(sf::IntRect(64*4, 64*5, 64, 64));
		sf::Vector2<unsigned int> size = block_texture.getSize();
//		block_sprite.setOrigin(size.x/2, size.y/2);
//		float scale = 1.0;
	} else return false;
	
	zoom = 100;
	
	return true;
}

bool ClientApp::loop () {
	
	// VIEW TRANSFORM ////////////////////////////////////////////////////////////////////
	int w = window.getSize().x;
	int h = window.getSize().y;
	float screen_aspect = float(w)/float(h);
	sf::View view;
	view.setCenter(0, 0);
	view.setSize(screen_aspect*zoom, -zoom);
	view.setViewport(sf::FloatRect(0, 0, 1, 1));
	window.setView(view);
	
	// INPUT ////////////////////////////////////////////////////////////////////////////
	handleInput();
	if (!running) return false;

	// SIMULATE /////////////////////////////////////////////////////////////////////////
	// TODO
	
	// DRAW //////////////////////////////////////////////////////////////////////////////
	window.clear();
	
	sf::Color GREEN(40, 240, 40, 255);
	block_sprite.setColor(GREEN);
	block_sprite.rotate(30.0/60);
	block_sprite.setOrigin(8, 8);
	block_sprite.setScale(3.0/16.0, 3.0/16.0);
//	block_sprite.setScale(0.02, 1);
	window.draw(block_sprite);
	
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
//					case sf::Mouse::Left:       finalizeObject(); break;
//					case sf::Mouse::Right:      placePoint(); break;
				}
				break;
			case sf::Event::MouseWheelMoved:
				zoom *= pow(1.1, event.mouseWheel.delta);
				zoom = min(max(zoom, 10.0f), 100.0f);
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Escape:  running = false; return;
				}
				break;
		}
	}
}

int main (int argc, char const** argv) {
	ClientApp app;
	return app.go(argc, argv);
}
