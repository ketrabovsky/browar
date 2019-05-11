#pragma once

#include <string>

class Connection
{
private:
	int socketfd;
public:
	Connection(int sockfd);
	int send_data(const std::string &buffer);
	int recieve(char *buffer, const int buff_size);
};
