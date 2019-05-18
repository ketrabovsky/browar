#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "ConfigParser.hpp"
#include "StringUtils.hpp"
#include "server.hpp"
#include "relayManager.hpp"
#include "relay.hpp"
#include "Json.hpp"
#include "fileReader.hpp"

#define MAX_BUF_SIZE 1024


namespace Commands 
{
    const std::string CMD_GET = "GET";
    const std::string CMD_SET = "SET";
    const std::string CMD_QUIT = "QUIT";
    
    enum
    {
        GET,
        SET,
        QUIT,
        UNKNOWN,
    };
}


namespace Config 
{
    const std::string config_filename = "config.json";
}



struct cmd_struct
{
    int cmd; // command SET, GET, QUIT
    std::string periph_name; // arg 1 name of perpherial to use
    int arg2; // arg 2 if cmd is SET this is ON or OFF
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
//      @data - input data stream
// output:
//      @tokens - input split by space ' '
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

void parse_command(std::vector<std::string> &tokens, cmd_struct &cmd_s)
{
    int size = tokens.size();
    if (size == 1) // might be quit
    {
        if (Commands::CMD_QUIT == tokens[0])
        {
            cmd_s.cmd = Commands::QUIT;
            return;
        }
    }
    else if (size == 2)  // migth be GET
    {
        if (Commands::CMD_GET == tokens[0])
        {
            cmd_s.cmd = Commands::GET;
        }

        auto value1 = tokens[1];
        cmd_s.periph_name = value1;
        return;

    }
    else if (size == 3) // might be SET
    {
        if (Commands::CMD_SET == tokens[0])
        {
            cmd_s.cmd = Commands::SET;
        }

        auto value1 = tokens[1];
        cmd_s.periph_name = value1;
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
        case Commands::GET:
        {
            int res = manager.get_relay_state(cmd.periph_name);
            std::cout << "ARG is: " << cmd.periph_name << std::endl;
            std::cout << "RES IS: " << res << std::endl;
            if (res == RelayState::ON) response.status = "ON\n";
            else if (res == RelayState::OFF) response.status = "OFF\n";
            else response.status = "FAIL\n";
            response.cmd_status = 0;
            break;
        }
        case Commands::SET:
        {
            int res = manager.set_relay_state(cmd.periph_name, cmd.arg2);
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
        case Commands::QUIT:
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
    int bytes;
    int running = 1;
    cmd_struct cmd;
    result_struct result;
    
    while(running)
    {
        // clear buffer before recieve
        std::memset(buffer, 0, MAX_BUF_SIZE);
        bytes = recv(socketfd, buffer, MAX_BUF_SIZE, 0);
        if (bytes == -1)
        {
            std::cout << "Error during receiving data" << std::endl;
            close(socketfd);
            running = 0;
            continue;
        }

        // if received 0 bytes close socket
        if (bytes < 1) running = 0;
        std::cout << "Received data: :\"" << buffer << "\"" << std::endl;
        std::string data = buffer;
        std::vector<std::string> tokens;

        create_tokens(data, tokens);
        std::cout << "LENGTH OF TOKENS: " << tokens.size() << std::endl;
        if (tokens.size() < 1) continue;
        parse_command(tokens, cmd);
        
        execute_command(cmd, result, running);
        bytes = send(socketfd, result.status.c_str(), result.status.size(), 0);
        if (bytes == -1)
        {
            std::cout << "SOME ERROR OCCURED ON SENDING DATA" << std::endl;
        }
    
    }

    close(socketfd);
}

int main()
{
    const std::string therm_type = "therm";
    const std::string relay_type = "relay";

    const uint16_t port = 5050;

    const std::string cfg_file_content = FileUtils::FileReader::read_file(Config::config_filename);
    const auto json_config = Json::Json::parse_from_string(cfg_file_content);
    const auto periph_array = json_config["peripherals"].get_value_as_vector(); 

    RelayManager &relayManager = RelayManager::get_instance();

    for(const auto &periph: periph_array)
    {
        std::string name = periph["name"].get_value_as_string();
        std::string type = periph["type"].get_value_as_string();
        int pin = periph["pin"].get_value_as_int();

        if (therm_type == type)
        {
            // TODO: add thermometer manager class and handle this
        }
        else if (relay_type == type)
        {
            relayManager.add_relay(name, pin);
        }

        std::cout << "FOUND: " << name << " TYPE IS: " << type << std::endl;
    }

    Server server(port);
    server.start_listening(handle_function);

    return 0;
}
