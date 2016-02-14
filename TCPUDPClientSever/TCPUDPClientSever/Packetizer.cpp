#include "Packetizer.h"

/*
Unused class,
written by Jaeagr Sarauer.
Intended to be the packetizer, but went unused.
*/


//this gets called when a packet is successfully sent.
//it will pop the front packet off, and push on a new one.
void Packetizer::updatePacketList () {
	if (packet_list.size () > 0)
		packet_list.pop_front ();//erase (packet_list.begin ());
	if (packet_list.size () < PACKET_LIST_MAX && currentLine < getLines () - 1)
		appendPackets ();
}

//this function appends packets to the end of the packet_list.
void Packetizer::appendPackets () {
	while (packet_list.size() < PACKET_LIST_MAX)
		packet_list.push_back (buildPacket ());
}

//this function will add a single line from a edit box to the temp_buffer.
void Packetizer::populateBuffer () {
	//if (show_data)
		temp_buffer.append (getLine (currentLine++));
	//else
		//temp_buffer.append(readFile());
}

//checks to see if it can retrieve any more packets.
//returns true if there are no more packets to make.
bool Packetizer::outOfData () {
	// ----------- testing

	char t[512];
	_itoa_s (temp_buffer.length (), t, 10);
	OutputDebugString (t);
	// -------------------
	return (currentLine >= getLines () && temp_buffer.length () == 0);
}

//retrieves the first 512 characters from the temp_buffer and removes them from the buffer.
std::string Packetizer::buildPacket () {
	std::string tmp;
	while (packet_list.size () < PACKET_LIST_MAX && (temp_buffer.length () + getLine (currentLine).length () < packet_size * 4) && currentLine < getLines ()) {//&& outOfData ()) {
		populateBuffer ();
	}
	/*if (temp_buffer.length () < TEMP_BUFFER && currentLine >= getLines () - 1 && addEOT) {
		temp_buffer += ((char)4);
		addEOT = FALSE;
	}*/
	tmp = temp_buffer.substr (0, packet_size - 1);
	temp_buffer.erase (0, packet_size - 1);
	tmp += '\0';
	return tmp;
}

//returns the first packet im the packet_list. 
//Returns null if no packets left.
std::string Packetizer::getPacket () {
	return packet_list[0];//begin ();
}

bool Packetizer::availablePacket () {
	return (packet_list.size () > 0);
}
