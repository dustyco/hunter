

#ifndef _WIN32
	#include <csignal>
#endif
#include <iostream>
#include <SFML/Network.hpp>
#include "common_sfml_network.h"
using namespace std;


const float RECONNECT_DELAY = 0.5;  // Seconds
const float HEARTBEAT_DELAY = 0.2;   // Seconds
const float TIMEOUT_PERIOD = 5;   // Seconds


struct ClientNet
{
	enum Status
	{
		DISCONNECTED,
		RECEIVE_GREET_ACK,
		NORMAL
	};
	
	PlayerID      id;
	sf::TcpSocket tcp;
	sf::UdpSocket udp;
	sf::IpAddress server_address;
	string        server_hostname;
	float         ts_connect;
	float         timeout;
	Status        status;
	bool          disconnect;
	string        disconnect_reason;
	PilotControls pilot_controls;
	
	void ClientNet_init        ();
	void ClientNet_tick        (float dt);
	void handleDisconnected    ();
	void handleReceiveGreetAck ();
	void handleNormal          ();
	void sendControls          ();
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
	disconnect = false;
	id = 0;
	timeout = 0;
}

void ClientNet::ClientNet_tick (float dt)
{
	ts_connect += dt;
	
	// Accumulate time and see if we should disconnect
	timeout += dt;
	if (timeout>TIMEOUT_PERIOD) disconnect = true;
	
	switch (status) {
		case DISCONNECTED:      handleDisconnected(); break;
		case RECEIVE_GREET_ACK: handleReceiveGreetAck(); break;
		case NORMAL:            handleNormal(); break;
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
	sf::Packet greeting;
	greeting << net::GREET_NUMBER << net::GREET_VERSION;
	greeting << string("dustyco");  // TODO Player specified name
	if (tcp.send(greeting)==sf::Socket::Done) {
		cout << "Connected! Greeting sent, awaiting ack" << endl;
		status = RECEIVE_GREET_ACK;
	}
}

void ClientNet::handleReceiveGreetAck ()
{
	sf::Packet greeting_ack;
	sf::Socket::Status err = tcp.receive(greeting_ack);
	if (err==sf::Socket::Done) {
		timeout = 0;
		uint8_t msg_type;
		// Read it all
		if (!(greeting_ack >> msg_type) || !(greeting_ack >> id)) {
			disconnect = true;
			disconnect_reason = "Malformed greeting ack";
			cout << disconnect_reason << endl;
			return;
		}
		if (msg_type!=net::MSG_TYPE_GREET_ACK) {
			disconnect = true;
			disconnect_reason = "Unexpected message type";
			cout << disconnect_reason << endl;
			return;
		}
		status = NORMAL;
		cout << "Normal status" << endl;
	} else if (err==sf::Socket::Disconnected || err==sf::Socket::Error) {
		disconnect = true;
		return;
	}
}

void ClientNet::handleNormal ()
{
	sendControls();
}

void ClientNet::sendControls ()
{
	sf::Packet input;
	input << net::GREET_NUMBER << id;
	input << net::MSG_TYPE_CONTROLS;
	input << pilot_controls;
	if (udp.send(input, server_address, net::DEFAULT_PORT)==sf::Socket::Done) {
		cout << "Sent input state" << endl;
	}
}

