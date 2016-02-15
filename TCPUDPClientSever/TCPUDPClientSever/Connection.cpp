#include "Connection.h"

void CreateSocket () {

	//initiate the DLL for connections
	WORD version;
	version = MAKEWORD (2, 2);
	if (WSAStartup (version, &WSAData) != 0) {
		MessageBox (hwnd, "Couldn't find DLL!", "DLL Error", MB_OK);
	}

	//determine which scoket connection this should be (TCP or UDP).
	int addFam;
	int type;
	switch (proto_type) {
		case PROTOCOL_TCP:
			addFam = AF_INET;
			type = SOCK_STREAM;
			break;
		case PROTOCOL_UDP:
			addFam = PF_INET;
			type = SOCK_DGRAM;
			break;
		default:
			break;
	}

	//create the socket based on flags indicating TCP/UDP
	if ((ProgSocket = socket (addFam, type, 0)) == -1) {
		MessageBox (hwnd, "There was an error connecting to the socket.\nAre you already connected, or is another program using it?", "Error Connecting to Socket!", MB_OK);
	}

}


void CloseConnection () {
	Connection_Setup = FALSE;
	closesocket (ProgSocket);
	WSACleanup ();
}


long packetDelay(SYSTEMTIME t1, SYSTEMTIME t2) {
	long d;

	d = (t2.wSecond - t1.wSecond) * 1000;
	d += (t2.wMilliseconds - t1.wMilliseconds);
	return(d);
}