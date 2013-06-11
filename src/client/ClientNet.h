

#ifndef _WIN32
	#include <csignal>
#endif
#include <iostream>
#include <SFML/Network.hpp>
using namespace std;


const float RECONNECT_DELAY = 0.5;  // Seconds


struct ClientNet
{
	enum Status {
		DISCONNECTED,
		RECEIVE_GREET_ACK
	};
	
	sf::TcpSocket tcp;
	sf::UdpSocket udp;
	sf::IpAddress server_address;
	string        server_hostname;
	float         time_since_connect;
	Status        status;
	
	void ClientNet_init ();
	void ClientNet_tick (float dt);
	void handleDisconnected ();
	void handleReceiveGreetAck ();
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
	time_since_connect = RECONNECT_DELAY;
	status = DISCONNECTED;
}

void ClientNet::ClientNet_tick (float dt)
{
	time_since_connect += dt;
	switch (status) {
		case DISCONNECTED:      handleDisconnected(); break;
		case RECEIVE_GREET_ACK: handleReceiveGreetAck(); break;
	}
}

void ClientNet::handleDisconnected ()
{
	// Try a connection
	if (time_since_connect>=RECONNECT_DELAY) {
		cout << "Connecting" << endl;
		tcp.connect(server_address, net::DEFAULT_PORT);
		time_since_connect = 0;
	}
	
	// Try to send a greeting
	sf::Packet greeting;
	greeting << net::GREET_NUMBER << net::GREET_VERSION;
	greeting << net::GREET_NAME_TYPE_NEW << string("dustyco");
	if (tcp.send(greeting)==sf::Socket::Done) {
		cout << "Connected! Greeting sent, awaiting ack" << endl;
		status = RECEIVE_GREET_ACK;
	}
}

void ClientNet::handleReceiveGreetAck ()
{
	sf::Packet greeting_ack;
	switch (tcp.receive(greeting_ack)) {
		case sf::Socket::Done:
			cout << "Received greeting ack" << endl;
			break;
		case sf::Socket::NotReady:
//			cout << "NotReady" << endl;
			break;
		case sf::Socket::Disconnected:
			cout << "Disconnected" << endl;
			status = DISCONNECTED;
			break;
		case sf::Socket::Error:
			cout << "Error" << endl;
			status = DISCONNECTED;
			break;
	}
	
}


