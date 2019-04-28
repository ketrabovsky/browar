#pragma once

#define MAX_THREAD 5

#include <functional>
#include <thread>
#include <vector>

class Server
{
private:
	int socketfd;
	int connection_n;
	volatile int running;
	uint16_t port;
	std::vector<std::thread> threads;
public:
	Server(uint16_t port);
	~Server();
	void start_listening(std::function<void(int)> handle_fn);
	void stop();
	int get_socketfd();
	int get_connection_n();
	uint16_t get_port();
};
