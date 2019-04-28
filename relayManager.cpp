#include "relayManager.h"

#include <wiringPi.h>
#include <iostream>


RelayManager& RelayManager::get_instance()
{
	static RelayManager manager;

	return manager;
}

RelayManager::RelayManager()
{
	int error;
	this->mleko = 1;
	error = wiringPiSetup();
	if (error)
	{
		std::cout << "Error during wirinPiSetup" << std::endl;
	}
}

int RelayManager::add_relay(const std::string &name, int pin)
{
	this->relays.emplace(name, pin);
	this->set_relay_state(name, RelayState::OFF);
}

int RelayManager::get_relay_state(const std::string &name)
{
	auto rl = this->relays.find(name);
	if (rl == this->relays.end())
	{
		return -1;
	}

	return rl->second.get_state();
}

int RelayManager::set_relay_state(const std::string &name, int state)
{
	auto rl = this->relays.find(name);
	if (rl == this->relays.end())
	{
		return -1;
	}

	rl->second.set_state(state);
	return 0;
}

int RelayManager::remove_relay(const std::string &name)
{
	auto rl = this->relays.find(name);
	if (rl == this->relays.end())
	{
		return -1;
	}

	this->relays.erase(rl);
	return 0;
}

