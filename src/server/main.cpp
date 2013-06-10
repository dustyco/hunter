

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>
#include "common.h"
#include "ServerSim.h"
using namespace std;


const double TICK_RATE = 50;
const double TICK_PERIOD = 1.0/TICK_RATE;


struct ServerApp : ServerSim
{
	bool         running;
	sf::Clock    clock;
	
	int  go      (int argc, char const** argv);
	bool setup   (int argc, char const** argv);
	bool loop    ();
	bool cleanup ();
};


// The program starts and ends here
int ServerApp::go (int argc, char const** argv)
{
	if (!setup(argc, argv)) {
		cout << "Error: ServerApp::setup() returned false" << endl;
		return 1;
	}
	while (loop()) {}
	if (!cleanup()) {
		cout << "Error: ServerApp::cleanup() returned false" << endl;
		return 1;
	}
	
	return 0;
}

bool ServerApp::setup (int argc, char const** argv)
{
	// True until we're ready to exit
	running = true;
	
	ServerSim_init();
	
	return true;
}

bool ServerApp::loop ()
{
	clock.restart();

	if (!running) return false;

	// SIMULATE /////////////////////////////////////////////////////////////////////////
	ServerSim_tick(TICK_PERIOD);
	
	// Block till it's time for the next loop
	sf::sleep( sf::microseconds(sf::Int64(1e6*TICK_PERIOD)) - clock.getElapsedTime() );
	
	return true;
}

bool ServerApp::cleanup ()
{
	return true;
}


int main (int argc, char const** argv)
{
	ServerApp app;
	return app.go(argc, argv);
}
