#ifndef PACKETIZER_H
#define PACKETIZER_H

#include <string>
#include <deque>
#include "Shared.h"

/*
Unused class,
written by Jaeagr Sarauer.
Intended to be the packetizer, but went unused.
*/

//Packet builder temp 
#define PACKET_LIST_MAX	12


class Packetizer {

	public:

	Packetizer (long curLine = 0) : currentLine (curLine) {};

	//this gets called when a packet is successfully sent.
	//it will pop the front packet off, and push on a new one.
	void updatePacketList ();

	//this function appends packets to the end of the packet_list.
	void appendPackets ();

	//this function will add a single line from a edit box to the temp_buffer.
	void populateBuffer ();

	//checks to see if it can retrieve any more packets.
	//returns true if there are no more packets to make.
	bool outOfData ();

	//retrieves the first 512 characters from the temp_buffer and removes them from the buffer.
	std::string buildPacket ();
	//returns the first packet im the packet_list. 
	//Returns null if no packets left.
	std::string getPacket ();

	//returns true if there is a packet in the list.
	bool availablePacket ();

	private:
	long currentLine;
	std::deque<std::string> packet_list;
	std::string temp_buffer;
	LPSTR file;
	bool doneReading = FALSE;
};


#endif