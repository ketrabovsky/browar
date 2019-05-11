#include "relay.hpp"

#include <wiringPi.h>

static void pinWrite(uint8_t, uint8_t);


static void writePin(uint8_t pin, uint8_t data)
{
	data &= 0x1;
	digitalWrite(pin, data);
}

Relay::Relay(int pin)
{
	this->pin = pin;
	this->state = OFF;
	pinMode(this->pin, OUTPUT);
	this->set_off();
}

void Relay::switch_relay()
{
	if (this->state == OFF)
	{
		this->state = ON;
		this->set_on();
	} else
	{
		this->state = OFF;
		this->set_off();
	}
}

int Relay::get_state()
{
	return this->state;
}

void Relay::set_on()
{
	this->state = ON;
	digitalWrite(this->pin, LOW);
}

void Relay::set_off()
{
	this->state = OFF;
	digitalWrite(this->pin, HIGH);
}

void Relay::set_state(int state)
{
	switch(state)
	{
		case ON:
			this->set_on();
			break;
		case OFF:
			this->set_off();
			break;
	}
}
