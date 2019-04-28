#include "connection.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>

Connection::Connection(int sockfd)
{
	this->socketfd = sockfd;
}

int Connection::send_data(const std::string &buffer)
{
	int bytes = send(this->socketfd, buffer.c_str(), buffer.size(), 0);
	return bytes;
}

int Connection::recieve(char *buffer, const int buff_size)
{
	// clears buffer first
	std::memset(buffer, 0, buff_size);
	int bytes = recv(this->socketfd, buffer, buff_size, 0);
	return bytes;
}
