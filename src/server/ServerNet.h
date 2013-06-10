

#pragma once
#ifndef _WIN32
	#include <csignal>
#endif
#include <iostream>
#include <SFML/Network.hpp>
#include "Sim.h"
using namespace std;


struct ServerNet
{
	typedef list<sf::TcpSocket*> TcpSocketList;
	
	sf::TcpListener tcp_listener;
	TcpSocketList   tcp_list;
	sf::UdpSocket   udp;
	
	bool ServerNet_init ();
	void ServerNet_tick (float dt);
};

bool ServerNet::ServerNet_init ()
{
	// BIND TCP AND UDP SOCKETS ////////////////////////////////////////
	#ifndef _WIN32
		signal(SIGPIPE, SIG_IGN);
	#endif
	if (tcp_listener.listen(PORT)!=sf::Socket::Done) {
		cout << "Error: Could not bind to TCP port " << PORT << endl;
		return false;
	}
	if (udp.bind(PORT)!=sf::Socket::Done) {
		cout << "Error: Could not bind to UDP port " << PORT << endl;
		return false;
	}
	cout << "Listening on port " << PORT << endl;
	tcp_listener.setBlocking(false);
	udp.setBlocking(false);
	
	return true;
}

void ServerNet::ServerNet_tick (float dt)
{
	// ACCEPT NEW CONNECTIONS //////////////////////////////////////////
	while (true) {
		sf::TcpSocket* client = new sf::TcpSocket;
		if (tcp_listener.accept(*client) == sf::Socket::Done) {
			std::cout << "New connection from " << client->getRemoteAddress() << std::endl;
			client->setBlocking(false);
			tcp_list.push_back(client);
		} else {
			delete client;
			break;
		}
	}

}
