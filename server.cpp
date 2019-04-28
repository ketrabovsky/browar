#include "server.h"

#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


Server::Server(uint16_t port)
{
	this->socketfd = 0;
	this->connection_n = 0;
	this->running = 0;
	this->port = port;

	sockaddr_in addres;
	socklen_t sockSize;
	int error;

	// create UNIX socket
	this->socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketfd == -1)
	{
		std::cout << "Couldnt create socket" << std::endl; 
		return;
	}

	// Specify addres of socket
	addres.sin_family = AF_INET;
	addres.sin_port = htons(this->port); // converts host order to network order
	addres.sin_addr.s_addr = INADDR_ANY;

	sockSize = sizeof(addres);

	error = bind(this->socketfd, (sockaddr *) &addres, sockSize);
	if (error)
	{
		std::cout << "Couldnt bind socket to port" << std::endl;
		return;
	}

	this->running = 1;

}

Server::~Server()
{
	close(this->socketfd);
}

void Server::start_listening(std::function<void(int)> handle_fn)
{
	int error;
	sockaddr_in addres;
	socklen_t sockSize;
	
	sockSize = sizeof(addres);

	// max 5 connections in queue
	error = listen(socketfd, 5);
	if (error)
	{
		this->running = 0;
	}

	while(this->running)
	{
		int client_socket = accept(this->socketfd, (sockaddr *) &addres, &sockSize);

		if (client_socket == -1)
		{
			std::cout << "Couldn't accept connection" << std::endl;
			continue;
		}

		this->threads.emplace(this->threads.end(), handle_fn, client_socket);
	
	}

	for (auto &t : this->threads)
	{
		t.join();
	}

}

void Server::stop()
{
	this->running = 0;
}

int Server::get_socketfd()
{
	return this->socketfd;
}

int Server::get_connection_n()
{
	return this->connection_n;
}

uint16_t Server::get_port()
{
	return this->port;
}
