/*
	Phillip Killewald
	1/31/08
*/

#include "Chamber.h"

using namespace std;

Chamber::Chamber():
	endcap(""),
	station(0),
	type(0),
	number(0),
	fiberCassetteCrate_(0),
	fiberCassettePos_(0),
	fiberCassetteSocket_(""),
	peripheralCrateId_(0),
	peripheralCrateVMECrate_(0),
	peripheralCrateVMESlot_(0)
{
	// All that for nothing...
}

Chamber::~Chamber()
{
	// Nothing.
}

/**
@returns a human-readable chamber name, like ME+1/2/12
**/
string Chamber::name()
{
	stringstream nameStream;
	//nameStream << "ME" << endcap << station << "/" << type << "/" << number;
	nameStream << endcap << station << "/" << type << "/" << setw(2) << setfill('0') << number;
	return nameStream.str();
}

/**
@returns a human-readable peripheral crate/slot name, like +1/03 slot 2
**/
string Chamber::peripheralCrate()
{
	stringstream nameStream;
	nameStream << endcap << station << "/" << setw(2) << setfill('0') << peripheralCrateVMECrate_ << " slot " << peripheralCrateVMESlot_;
	return nameStream.str();
}

/**
@returns a human-readable fiber cassette name, like 1/2/b or something...
**/
string Chamber::fiberCassette()
{
	stringstream nameStream;
	nameStream << fiberCassetteCrate_ << "/" << fiberCassettePos_ << "/" << fiberCassetteSocket_;
	return nameStream.str();
}