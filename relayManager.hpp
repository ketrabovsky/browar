#pragma once

#include <map>
#include <string>

#include "relay.hpp"

class RelayManager
{
private:
	std::map<std::string, Relay> relays;
	RelayManager();
public:
	RelayManager(const RelayManager&) = delete;
	RelayManager& operator=(const RelayManager&) = delete;
	static RelayManager& get_instance();	
	
	int add_relay(const std::string &name, int pin);
	int get_relay_state(const std::string &name);
	int set_relay_state(const std::string &name, int state);
	int remove_relay(const std::string &name);
};
