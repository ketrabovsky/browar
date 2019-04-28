#pragma once

#include <stdint.h>

enum RelayState 
{
	ON,
	OFF,
};

class Relay
{
private:
	// this describes pin to be used
	int pin;

	// this tells which on which state currently is relay
	int state;

public:
	Relay(int pin);
	void switch_relay();
	void set_on();
	void set_off();
	void set_state(int state);
	int get_state();
};
