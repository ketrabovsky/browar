#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "ConfigParser.h"
#include "StringUtils.h"
#include "server.h"
#include "relayManager.h"
#include "relay.h"

#define MAX_BUF_SIZE 1024


const std::string CMD_GET = "GET";
const std::string CMD_SET = "SET";
const std::string CMD_QUIT = "QUIT";


const std::string config_filename = "config.cfg";

static std::map<int, std::string> relay_map;

enum
{
	GET,
	SET,
	QUIT,
	UNKNOWN,
};

struct cmd_struct
{
	int cmd; // command SET, GET, QUIT
	int arg1; // arg 1 number of relay
	int arg2; // arg 2 if cmd is SET this is ON or OFF
};

enum
{
	pompa1,
	pompa2,
};

struct result_struct
{
	int cmd_status;
	std::string status;
};


// this function converts input data to std::vector<std::string> each word
// is separated by space and end of data is \n character, opther whitespace
// characters are stripped
// input:
// 		@data - input data stream
// output:
// 		@tokens - input split by space ' '
void create_tokens(std::string &data, std::vector<std::string> &tokens)
{
	auto pos = data.find("\n");
	if (pos != std::string::npos)
	{
		data.erase(pos, 1);
	}
	pos = data.find("\r");
	if (pos != std::string::npos)
	{
		data.erase(pos, 1);
	}

	// data should be "word word word" split by name
	while(1)
	{
		auto pos = data.find(' ');
		if (pos != std::string::npos)
		{
			// this pushes argument
			tokens.push_back(data.substr(0, pos));
			data.erase(0, pos + 1);
		}
		else
		{
			if (data.size() > 0)
			{
				tokens.push_back(data);
			}
			break;
		}
	}
}

// this reads file content and ommits data which is preceded by commenChar
// input:
// 		@filename - this is filename of file which should be read
//  	@commentchar - this tells which character is used to create comments
//			after this character line isn't read
// output:
//		@content - reference to which file is written, each line ends with \n character
// 			even if endLine is \r\n
void read_file(std::string &content, const std::string &filename, const char commentchar = '#')
{
	std::fstream file;
	file.open(filename, std::fstream::in);

	if (!file.good())
	{
		std::cout << "couldn't open config file" << std::endl;
		return;
	}

	while(!file.eof())
	{
		std::string line;
		std::getline(file, line);
		StringUtils::cut_string(line, commentchar);
		StringUtils::strip_whitespaces(line);
		if (line.size() < 1) continue;
		line.append("\n");
		content.append(line.c_str());
	}

	file.close();
}

void parse_command(std::vector<std::string> &tokens, cmd_struct &cmd_s)
{
	int size = tokens.size();
	if (size == 1) // might be quit
	{
		if (0 == tokens[0].compare(CMD_QUIT))
		{
			cmd_s.cmd = QUIT;
			return;
		}
	}
	else if (size == 2)  // migth be GET
	{
		if (0 == tokens[0].compare(CMD_GET))
		{
			cmd_s.cmd = GET;
		}

		auto value1 = std::stoi(tokens[1]);
		cmd_s.arg1 = value1;
		return;

	}
	else if (size == 3) // might be SET
	{
		if (0 == tokens[0].compare(CMD_SET))
		{
			cmd_s.cmd = SET;
		}

		auto value1 = std::stoi(tokens[1]);
		cmd_s.arg1 = value1;
		if (0 == tokens[2].compare(std::string("ON")))
		{
			cmd_s.arg2 = RelayState::ON;
		}
		else if (0 == tokens[2].compare(std::string("OFF")))
		{
			cmd_s.arg2 = RelayState::OFF;
		}
		else
		{
			cmd_s.arg2 = -1;
		}

		return;

	}
	else
	{
		std::cout << "Unknown command while parsing" << std::endl;	
		return;
	}
}

void execute_command(cmd_struct &cmd, result_struct &response, int &running)
{

	RelayManager &manager = RelayManager::get_instance();

	switch(cmd.cmd)
	{
		case GET:
		{
			int res = manager.get_relay_state(relay_map[cmd.arg1]);
			std::cout << "ARG is: " << cmd.arg1 << std::endl;
			std::cout << "Mapped is : " << relay_map[cmd.arg1] << std::endl;
			std::cout << "RES IS: " << res << std::endl;
			if (res == RelayState::ON) response.status = "ON\n";
			else if (res == RelayState::OFF) response.status = "OFF\n";
			else response.status = "FAIL\n";
			response.cmd_status = 0;
			break;
		}
		case SET:
		{
			int res = manager.set_relay_state(relay_map[cmd.arg1], cmd.arg2);
			if (res == -1)
			{
				response.cmd_status = -1;
				response.status = "SET CMD HAS FAILED\n";
			}
			else
			{
				response.cmd_status = 0;
				response.status = "OK\n";
			}
			break;
		}
		case QUIT:
		{
			running = 0;
			response.cmd_status = 0;
			response.status = "quiting\n";
			break;
		}
		default:
		{
			response.cmd_status = -1;
			response.status = "unknown cmd\n";
			break;
		}
	}
}

void handle_function(int socketfd)
{
	char buffer[MAX_BUF_SIZE];
	int error;
	int running = 1;
	cmd_struct cmd;
	result_struct result;
	
	while(running)
	{
		// clear buffer before recieve
		std::memset(buffer, 0, MAX_BUF_SIZE);
		error = recv(socketfd, buffer, MAX_BUF_SIZE, 0);
		if (error == -1)
		{
			std::cout << "Error during receiving data" << std::endl;
			close(socketfd);
			running = 0;
			continue;
		}
		std::string data = buffer;
		std::vector<std::string> tokens;

		create_tokens(data, tokens);
		parse_command(tokens, cmd);
		
		execute_command(cmd, result, running);
		send(socketfd, result.status.c_str(), result.status.size(), 0);
	
	}

	close(socketfd);
}

int main()
{
	int relay_number = 0;
	
	const std::string relay1 = "relay1";
	const std::string relay2 = "relay2";
	const uint16_t port = 5050;

	std::string cfg_file_content;
	std::string basic_relay_name = "relay";	

	read_file(cfg_file_content, config_filename, '#');
	ConfigParser cfg(cfg_file_content);
	
	const int relay1_pin = cfg.get_value_as_int(relay1);
	const int relay2_pin = cfg.get_value_as_int(relay2);

	relay_map[relay_number++] = relay1;
	relay_map[relay_number++] = relay2;
	

	RelayManager &relayManager = RelayManager::get_instance();


	relayManager.add_relay(relay1, relay1_pin);
	relayManager.add_relay(relay2, relay2_pin);

	Server server(port);
	server.start_listening(handle_function);

	return 0;
}
