

#pragma once
#ifndef _WIN32
	#include <csignal>
#endif
#include <sstream>
#include <iostream>
#include <SFML/Network.hpp>
#include "common.h"
#include "common_sfml_network.h"
#include "Sim.h"
#include "PlayerDB.h"
using namespace std;


const float CLIENT_TIMEOUT_PERIOD = 3;  // Seconds


struct ServerNet
{
	enum Status
	{
		GET_GREETING,
		ACK_GREETING,
		NORMAL,
		DISCONNECT,
		DESTROY
	};
	
	struct Client : private NonCopyable
	{
		PlayerID      id;
		float         timeout;
		string        disconnect_reason;
		Status        status;
		sf::TcpSocket tcp;
		PilotControls pilot_controls;
		
		     Client     () : timeout(0), status(GET_GREETING) {}
		void disconnect (const string& reason);
	};
	typedef list<Client*> ClientList;
	typedef map<PlayerID,Client*> ClientMap;
	
	sf::TcpListener tcp_listener;
	sf::UdpSocket   udp;
	ClientList      clients_connecting;
	ClientMap       clients;
	
	bool ServerNet_init    ();
	void ServerNet_cleanup ();
	void ServerNet_tick    (float dt);
	void readUDP           ();
	void handleGetGreeting (Client& client);
	void handleAckGreeting (Client& client);
	void handleDisconnect  (Client& client);
};

void ServerNet::Client::disconnect (const string& reason)
{
	if (status == DESTROY) return;
	PlayerDB& player_db = PlayerDB::getSingleton();
	disconnect_reason = reason;
	status = DISCONNECT;
	if (player_db.has(id)) player_db.get(id).online = false;
}

bool ServerNet::ServerNet_init ()
{
	PlayerDB::getSingleton().load("gamestate.txt");
	
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
	PlayerDB::getSingleton().save("gamestate.txt");
}

void ServerNet::ServerNet_tick (float dt)
{
	// ACCEPT NEW CONNECTIONS //////////////////////////////////////////
	while (true) {
		Client* client = new Client;
		if (tcp_listener.accept(client->tcp) == sf::Socket::Done) {
			std::cout << "New connection from " << client->tcp.getRemoteAddress() << std::endl;
			client->tcp.setBlocking(false);
			clients_connecting.push_back(client);
		} else {
			delete client;
			break;
		}
	}
	
	// Each connectING client connection
	for (ClientList::iterator client_it=clients_connecting.begin(); client_it!=clients_connecting.end();) {
		Client& client = *(*client_it);
		
		if (client.status==NORMAL) {
			// Put it into the connected group
			clients[client.id] = *client_it;
			clients_connecting.erase(client_it++);
			continue;
		}
		
		// Accumulate time and see if we should disconnect
		client.timeout += dt;
		if (client.timeout>CLIENT_TIMEOUT_PERIOD) client.disconnect("Timed out while connecting");
		
		switch (client.status) {
			case GET_GREETING: handleGetGreeting(client); break;
			case ACK_GREETING: handleAckGreeting(client); break;
			case DISCONNECT:   handleDisconnect(client); break;
			case DESTROY:
				cout << "destroy" << endl;
				delete *client_it;
				clients_connecting.erase(client_it);
				break;
		}
		++client_it;
	}
	
	// Each connectED client connection
	for (ClientMap::iterator client_it=clients.begin(); client_it!=clients.end();) {
		Client& client = *(client_it->second);
		
		// Accumulate time and see if we should disconnect
		client.timeout += dt;
		if (client.timeout>CLIENT_TIMEOUT_PERIOD) client.disconnect("Timed out");
		
		switch (client.status) {
			case DISCONNECT:   handleDisconnect(client); break;
			case DESTROY:
				cout << "destroy" << endl;
				delete client_it->second;
				clients.erase(client_it);
				break;
		}
		++client_it;
	}
	
	// Read UDP messages
	readUDP();
}

void ServerNet::readUDP ()
{
	PlayerDB& player_db = PlayerDB::getSingleton();
	
	sf::Packet     packet;
	sf::IpAddress  remote_address;
	unsigned short remote_port;
	while (udp.receive(packet, remote_address, remote_port) == sf::Socket::Done)
	{
		// Read header
		uint32_t greet_number;
		PlayerID id;
		uint8_t  msg_type;
		if (
			!(packet >> greet_number >> id >> msg_type) ||
			greet_number!=net::GREET_NUMBER
		) continue;
		
		// Find the cooresponding client
		ClientMap::iterator client_it = clients.find(id);
		if (client_it != clients.end())
		{
			Client& client = *client_it->second;
			
			// Reset the client's timeout
			client.timeout = 0;
			
			// Handle message
//			cout << "UDP from " << remote_address << ":" << remote_port << endl;
			PilotControls pilot_controls;
			if (msg_type==net::MSG_TYPE_CONTROLS && (packet >> pilot_controls)) {
				// Update the client's controls
				// TODO This may not be necessary (if so, remove variable from Client)
				client.pilot_controls = pilot_controls;
				
				// Update the player's controls
				if (player_db.has(id)) player_db.get(id).pilot_controls = pilot_controls;
			}
		}
		else continue;
		
	}
}

void ServerNet::handleGetGreeting (Client& client)
{
	PlayerDB& player_db = PlayerDB::getSingleton();
	
	sf::Packet greeting;
	sf::Socket::Status err = client.tcp.receive(greeting);
	if (err==sf::Socket::Done) {
		client.timeout = 0;
		uint32_t greet_number, greet_version;
		string greet_name;
		// Read it all
		if (
			!(greeting >> greet_number) ||
			!(greeting >> greet_version) ||
			!(greeting >> greet_name)
		) {
			client.disconnect("Malformed greeting message");
			return;
		}
		if (greet_number!=net::GREET_NUMBER) {
			client.disconnect("Invalid greeting number");
			return;
		}
		if (greet_version!=net::GREET_VERSION) {
			stringstream ss;
			ss << "Version mismatch (client: " << greet_version << ", server: " << net::GREET_VERSION << ")";
			client.disconnect(ss.str());
			return;
		}
		if (greet_name.empty()) {
			client.disconnect("Blank name request");
			return;
		}
		// Add the player if it's not already registered
		if (!player_db.has(greet_name)) {
			player_db.add(PlayerInfo(player_db.issueID(), greet_name));
			cout << "Registered a new player: " << greet_name << endl;
			player_db.save("gamestate.txt");
		}
		// Proceed
		PlayerInfo& info = player_db.get(greet_name);
		// See if the player is already online
		if (info.online) {
			client.disconnect("Player is already connected");
			return;
		}
		cout << greet_name << " connected" << endl;
		client.id = info.id;
		client.status = ACK_GREETING;
		handleAckGreeting(client);
	} else if (err==sf::Socket::Disconnected || err==sf::Socket::Error) {
		client.disconnect("Connection closed");
	}
}

void ServerNet::handleAckGreeting (Client& client)
{
	sf::Packet packet;
	packet << net::MSG_TYPE_GREET_ACK;
	packet << client.id;
	
	sf::Socket::Status err = client.tcp.send(packet);
	if (err==sf::Socket::Done) {
		cout << "Greeting ack sent" << endl;
		client.status = NORMAL;
	} else if (err==sf::Socket::Disconnected || err==sf::Socket::Error) {
		client.disconnect("Connection closed");
	}
}

void ServerNet::handleDisconnect (Client& client)
{
	PlayerDB& player_db = PlayerDB::getSingleton();
	
	// Print who and why
	if (player_db.has(client.id)) cout << player_db.get(client.id).name;
	else                          cout << client.tcp.getRemoteAddress();
	if (client.disconnect_reason.empty()) cout << " disconnected" << endl;
	else                                  cout << " disconnected: " << client.disconnect_reason << endl;
	
	sf::Packet packet;
	packet << net::MSG_TYPE_DISCONNECT;
	packet << client.disconnect_reason;
	
	sf::Socket::Status err = client.tcp.send(packet);
	if (err==sf::Socket::Done) {
		cout << "Disconnect message sent" << endl;
		client.status = DESTROY;
	} else if (err==sf::Socket::Disconnected || err==sf::Socket::Error) {
		cout << "Couldn't send disconnect message (disconnected or error)" << endl;
		client.status = DESTROY;
	} else {
		cout << "Couldn't send disconnect message" << endl;
	}
}


