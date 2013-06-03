

/*
	Main loop, input, drawing, interface with SFML
*/


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
	
	int  go          (int argc, char const** argv);
	bool setup       (int argc, char const** argv);
	bool loop        ();
	bool cleanup     ();
	
	void handleInput ();
//	void drawLine    (const Vec& a, const Vec& b);
//	void drawPoly    (const Poly& poly, sf::Color color, bool outline = false, bool points = false);
//	void drawPoint   (const Vec& point, sf::Color color);
//	void drawObject  (const Object& object, sf::Color color);
//	Vec  viewTransformInverse (const Vec& v);
};

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
	
	return true;
}

bool ClientApp::loop () {
	
	// VIEW TRANSFORM ////////////////////////////////////////////////////////////////////
	int w = window.getSize().x;
	int h = window.getSize().y;
	float screen_aspect = float(w)/float(h);
	float zoom = 5;
	// World position and size of view frame
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(screen_aspect*zoom, zoom));
//	view.reset(sf::FloatRect(-screen_aspect/2, 0.5, screen_aspect, -1));
	// Cover whole window
	view.setViewport(sf::FloatRect(0, 0, 1, 1));
	window.setView(view);
	
	// INPUT ////////////////////////////////////////////////////////////////////////////
	handleInput();
	if (!running) return false;

	// SIMULATE /////////////////////////////////////////////////////////////////////////
	cout << "Simulation goes here" << endl;
	
	// DRAW //////////////////////////////////////////////////////////////////////////////
	window.clear();
	
//	line_sprite.rotate(dt*30);
//	line_sprite.setOrigin(0, 1.5);
//	line_sprite.setScale(1, 1.0/h*zoom*2);
//	line_sprite.setScale(0.02, 1);
//	window.draw(line_sprite);

	block_sprite.rotate(3.14159*2/60/3);
//	block_sprite.setOrigin(0, 1.5);
	block_sprite.setScale(1, 1.0/h*zoom*2);
	block_sprite.setScale(0.02, 1);
	window.draw(block_sprite);
	
	return true;
}

bool ClientApp::cleanup () {
	return true;
}

void ClientApp::handleInput ()
{
	// State based controls
//	grab = sf::Mouse::isButtonPressed(sf::Mouse::Left);
//	sf::Vector2i sf_mouse_pos = sf::Mouse::getPosition(window);
//	mouse = viewTransformInverse(Vec(sf_mouse_pos.x, sf_mouse_pos.y));
	
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
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Escape:  running = false; return;
				}
				break;
		}
	}
}
/*
Vec ClientApp::viewTransformInverse (const Vec& v) {
	Vec r;
	sf::Vector2u win = window.getSize();
	float screen_aspect = float(win.y)/win.x;
	r.x = (v.x-win.x/2)/win.y*zoom;
	r.y = (-v.y+win.y/2)/win.y*zoom;
	return r;
}

void ClientApp::drawLine (const Vec& a, const Vec& b) {
	line_sprite.setOrigin(0, 1.5);
	line_sprite.setPosition(a.x, a.y);
	line_sprite.setScale(length(b-a), 1.0/window.getSize().y*zoom*2.5);
	line_sprite.setRotation(angle_aa(b-a)*57.29578);
	window.draw(line_sprite);
}

void ClientApp::drawPoly (const Poly& poly, sf::Color color, bool outline, bool points) {
	if (poly.size()>=2) {
		sf::ConvexShape sf_shape(poly.size());
		color.a = 150;
		sf_shape.setOutlineColor(color);
		color.a = 50;
		sf_shape.setFillColor(color);
	
		int i = 0;
		Vec last = *(--poly.end());
		for (Poly::const_iterator it = poly.begin(); it!=poly.end(); it++,i++) {
			sf_shape.setPoint(i, sf::Vector2f(it->x, it->y));
			Vec now = *it;
			Vec mid = (now+last)*0.5f;
			Vec dif = (now-last);
			Vec norm(-dif.y, dif.x);
			
			if (outline) drawLine(last, now);
			last = now;
		}
		window.draw(sf_shape);
	}
	if (points) {
		color.a = 250;
		for (Poly::const_iterator it = poly.begin(); it!=poly.end(); it++) {
			drawPoint(*it, color);
		}
	}
}

void ClientApp::drawPoint (const Vec& point, sf::Color color) {
	dot_sprite.setPosition(point.x, point.y);
	dot_sprite.setScale(1.0/window.getSize().y*zoom, 1.0/window.getSize().y*zoom);
	window.draw(dot_sprite);
}

void ClientApp::drawObject (const Object& object, sf::Color color) {
	Mat rot(object.rot);
	Poly p;
	for (Poly::const_iterator it = object.points.begin(); it!=object.points.end(); it++)
		p.push_back(rot*(*it) + object.pos);
	drawPoly(p, color, true, false);
	drawPoint(object.pos, color);
}
*/

int main (int argc, char const** argv) {
	ClientApp app;
	return app.go(argc, argv);
}
