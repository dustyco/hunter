

#pragma once
#ifndef _WIN32
	#include <csignal>
#endif
#include <iostream>
#include <list>
#include <SFML/Network.hpp>
#include "common_sfml_network.h"
using namespace std;


const float RECONNECT_DELAY = 0.5;  // Seconds
const float HEARTBEAT_DELAY = 1;   // Seconds
const float TIMEOUT_PERIOD = 3;   // Seconds


struct ClientNet
{
	enum Status
	{
		DISCONNECTED,
		RECEIVE_GREET_ACK,
		NORMAL,
		DISCONNECT,
		QUIT
	};
	
	PlayerID      id;
	sf::TcpSocket tcp;
	sf::UdpSocket udp;
	sf::IpAddress server_address;
	string        server_hostname;
	float         ts_connect;
	float         timeout;
	float         heartbeat;
	Status        status;
	string        disconnect_reason;
	PilotControls pilot_controls;
	PacketList    ship_movement_packets;
	
	void ClientNet_init        ();
	void ClientNet_tick        (float dt);
	void handleDisconnected    ();
	void handleReceiveGreetAck ();
	void handleNormal          ();
	void handleDisconnect      ();
	void sendHeartbeat         ();
	void sendControls          ();
	void disconnect            (const string& reason);
};


void ClientNet::ClientNet_init ()
{
	// SET UP SOCKET /////////////////////////////////////////////////
	#ifndef _WIN32
		// Suppress process ending SIGPIPE signal from writing to a closed socket
		signal(SIGPIPE,SIG_IGN);
	#endif
	cout << "Server: " << server_hostname << ":" << net::DEFAULT_PORT << endl;
	server_address = sf::IpAddress(server_hostname);
	udp.bind(sf::Socket::AnyPort);
	tcp.setBlocking(false);
	udp.setBlocking(false);
	ts_connect = RECONNECT_DELAY;
	status = DISCONNECTED;
	id = 0;
	timeout = 0;
	heartbeat = 0;
}

void ClientNet::ClientNet_tick (float dt)
{
	ts_connect += dt;
	
	// Accumulate time and see if we should disconnect
	heartbeat += dt;
	timeout += dt;
	if (timeout>TIMEOUT_PERIOD) disconnect("Server: Timed out");
	
	switch (status) {
		case DISCONNECTED:      handleDisconnected(); break;
		case RECEIVE_GREET_ACK: handleReceiveGreetAck(); break;
		case NORMAL:            handleNormal(); break;
		case DISCONNECT:        handleDisconnect(); break;
	}
}

void ClientNet::handleDisconnected ()
{
	// Try a connection
	if (ts_connect>=RECONNECT_DELAY) {
		cout << "Connecting" << endl;
		tcp.connect(server_address, net::DEFAULT_PORT);
		ts_connect = 0;
	}
	
	// Try to send a greeting
	sf::Packet packet;
	packet << net::GREET_NUMBER << net::GREET_VERSION;
	packet << string("dustyco");  // TODO Player specified name
	if (tcp.send(packet)==sf::Socket::Done) {
		cout << "Connection established" << endl;
		status = RECEIVE_GREET_ACK;
	}
}

void ClientNet::handleReceiveGreetAck ()
{
	sf::Packet packet;
	sf::Socket::Status err = tcp.receive(packet);
	if (err==sf::Socket::Done)
	{
		timeout = 0;
		net::MsgType msg_type;
		// Read it all
		if (!(packet >> msg_type)) {
			disconnect("Malformed greeting ack");
			return;
		}
		switch (msg_type)
		{
			case net::MSG_TYPE_DISCONNECT:
				packet >> disconnect_reason;
				disconnect(disconnect_reason);
				return;
			case net::MSG_TYPE_GREET_ACK:
				if (packet >> id) status = NORMAL;
				else              disconnect("Malformed greeting ack");
				return;
		}
	}
	else if (err==sf::Socket::Disconnected || err==sf::Socket::Error)
	{
		disconnect("Connection closed");
		return;
	}
}

void ClientNet::handleNormal ()
{
	// Receive TCP messages
	while (true) {
		sf::Packet packet;
		sf::Socket::Status err = tcp.receive(packet);
		if (err==sf::Socket::Done)
		{
			timeout = 0;
			net::MsgType msg_type;
			if (!(packet >> msg_type)) disconnect("Malformed TCP message");
			switch (msg_type)
			{
				case net::MSG_TYPE_DISCONNECT:
					packet >> disconnect_reason;
					disconnect(disconnect_reason);
					continue;
			};
		} else if (err==sf::Socket::Disconnected || err==sf::Socket::Error) {
			disconnect("Connection closed");
		}
		break;
	}
	
	// Receive UDP messages
	sf::Packet     packet;
	sf::IpAddress  remote_address;
	unsigned short remote_port;
	while (udp.receive(packet, remote_address, remote_port) == sf::Socket::Done)
	{
		timeout = 0;
		net::MsgType msg_type;
		if (!(packet >> msg_type)) disconnect("Client: Malformed UDP message");
		switch (msg_type)
		{
			case net::MSG_TYPE_SHIP_MOVEMENT:
				ship_movement_packets.push_back(packet);
				break;
		};
	}
	
	// Send a TCP heartbeat packet if interval is passed
	if (heartbeat>HEARTBEAT_DELAY) sendHeartbeat();
	
	sendControls();
}

void ClientNet::handleDisconnect ()
{
	cout << "Disconnected: " << disconnect_reason << endl;
	
	sf::Packet packet;
	packet << net::MSG_TYPE_DISCONNECT;
	packet << disconnect_reason;
	
	sf::Socket::Status err = tcp.send(packet);
	if (err==sf::Socket::Done) {
//		cout << "Disconnect message sent" << endl;
		status = QUIT;
	} else if (err==sf::Socket::Disconnected || err==sf::Socket::Error) {
//		cout << "Couldn't send disconnect message (disconnected or error)" << endl;
		status = QUIT;
	} else {
//		cout << "Couldn't send disconnect message" << endl;
	}
}

void ClientNet::sendHeartbeat ()
{
	sf::Packet input;
	input << net::MSG_TYPE_HEARTBEAT;
	if (tcp.send(input) == sf::Socket::Done) {
		heartbeat = 0;
	}
}

void ClientNet::sendControls ()
{
	sf::Packet input;
	input << net::GREET_NUMBER << id;
	input << net::MSG_TYPE_CONTROLS;
	input << pilot_controls;
	if (udp.send(input, server_address, net::DEFAULT_PORT)==sf::Socket::Done) {
//		cout << "Sent input state" << endl;
	}
}

void ClientNet::disconnect (const string& reason)
{
	if (status == QUIT) return;
	disconnect_reason = reason;
	status = DISCONNECT;
}

