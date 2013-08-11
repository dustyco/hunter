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
