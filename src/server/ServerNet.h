

#pragma once
#ifndef _WIN32
	#include <csignal>
#endif
#include <sstream>
#include <iostream>
#include <SFML/Network.hpp>
#include "common.h"
#include "Sim.h"
#include "PlayerDB.h"
using namespace std;


const float CLIENT_TIMEOUT_PERIOD = 5;  // Seconds


struct ServerNet
{
	enum Status {
		GET_GREETING,
		GET_ID
	};
	
	struct Client : private NonCopyable
	{
		float         timeout;
		bool          disconnect;
		string        disconnect_reason;
		Status        status;
		sf::TcpSocket tcp;
		
		Client () : disconnect(false), timeout(0), status(GET_GREETING) {}
	};
	typedef list<Client*> ClientList;
	
	sf::TcpListener tcp_listener;
	sf::UdpSocket   udp;
	ClientList      clients;
	PlayerDB        player_db;
	
	bool ServerNet_init    ();
	void ServerNet_cleanup ();
	void ServerNet_tick    (float dt);
	void handleGetGreeting (Client& client);
};

bool ServerNet::ServerNet_init ()
{
	player_db.load("gamestate.txt");
//	player_db.add(PlayerInfo(player_db.issueID(), "UncleSamEagle"));
	
	// BIND TCP AND UDP SOCKETS ////////////////////////////////////////
	#ifndef _WIN32
		signal(SIGPIPE, SIG_IGN);
	#endif
	if (tcp_listener.listen(net::DEFAULT_PORT)!=sf::Socket::Done) {
		cout << "Error: Could not bind to TCP port " << net::DEFAULT_PORT << endl;
		return false;
	}
	if (udp.bind(net::DEFAULT_PORT)!=sf::Socket::Done) {
		cout << "Error: Could not bind to UDP port " << net::DEFAULT_PORT << endl;
		return false;
	}
	cout << "Listening on port " << net::DEFAULT_PORT << endl;
	tcp_listener.setBlocking(false);
	udp.setBlocking(false);
	
	return true;
}

void ServerNet::ServerNet_cleanup ()
{
	player_db.save("gamestate.txt");
}

void ServerNet::ServerNet_tick (float dt)
{
	// ACCEPT NEW CONNECTIONS //////////////////////////////////////////
	while (true) {
		Client* client = new Client;
		if (tcp_listener.accept(client->tcp) == sf::Socket::Done) {
			std::cout << "New connection from " << client->tcp.getRemoteAddress() << std::endl;
			client->tcp.setBlocking(false);
			clients.push_back(client);
		} else {
			delete client;
			break;
		}
	}
	
	// Each client connection
	for (ClientList::iterator client_it=clients.begin(); client_it!=clients.end(); ++client_it) {
		Client& client = *(*client_it);
		
		// Accumulate time and see if we should disconnect
		client.timeout += dt;
		if (client.timeout>CLIENT_TIMEOUT_PERIOD) {
			client.disconnect = true;
			client.disconnect_reason = "Timeout (you probably shouldn't be receiving this)";
		}
		
		switch (client.status) {
			case GET_GREETING: handleGetGreeting(client); break;
		}
	}
}

void ServerNet::handleGetGreeting (Client& client)
{
	sf::Packet greeting;
	sf::Socket::Status err = client.tcp.receive(greeting);
	if (err==sf::Socket::Done) {
		client.timeout = 0;
		uint32_t greet_number, greet_version, greet_name_type;
		string greet_name;
		// Read it all
		if (
			!(greeting >> greet_number) ||
			!(greeting >> greet_version) ||
			!(greeting >> greet_name_type) ||
			!(greeting >> greet_name)
		) {
			client.disconnect = true;
			client.disconnect_reason = "Malformed greeting message";
			cout << client.disconnect_reason << endl;
			return;
		}
		if (greet_number!=net::GREET_NUMBER) {
			client.disconnect = true;
			client.disconnect_reason = "Invalid greeting number";
			cout << client.disconnect_reason << endl;
			return;
		}
		if (greet_version!=net::GREET_VERSION) {
			client.disconnect = true;
			stringstream ss;
			ss << "Version mismatch (client: " << greet_version << ", server: " << net::GREET_VERSION << ")";
			client.disconnect_reason = ss.str();
			cout << client.disconnect_reason << endl;
			return;
		}
		if (greet_name.empty()) {
			client.disconnect = true;
			client.disconnect_reason = "Blank name request";
			cout << client.disconnect_reason << endl;
			return;
		}
		if (greet_name_type==net::GREET_NAME_TYPE_NEW) {
			// Validate the request
			if (player_db.has(greet_name)) {
				client.disconnect = true;
				client.disconnect_reason = "Player with that name already exists";
				cout << client.disconnect_reason << endl;
				return;
			}
			// Add the info
			player_db.add(PlayerInfo(player_db.issueID(), greet_name));
			cout << "Added a new player: " << greet_name << endl;
			player_db.save("gamestate.txt");
		} else if (greet_name_type==net::GREET_NAME_TYPE_EXISTING) {
			// Validate the request
			if (!player_db.has(greet_name)) {
				client.disconnect = true;
				client.disconnect_reason = "Player with that name does not exist";
				cout << client.disconnect_reason << endl;
				return;
			}
		} else {
			client.disconnect = true;
			client.disconnect_reason = "Unexpected ID message type";
			return;
		}
		// Proceed
		PlayerInfo& info = player_db.get(greet_name);
		
		
		return;
	} else if (err==sf::Socket::Disconnected || err==sf::Socket::Error) {
		client.disconnect = true;
		return;
	}
}

