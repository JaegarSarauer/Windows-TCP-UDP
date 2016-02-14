#include "Connection.h"

void CreateSocket () {

	//initiate the DLL for connections
	WORD version;
	version = MAKEWORD (2, 2);
	if (WSAStartup (version, &WSAData) != 0) {
		OutputDebugString ("DLL not found!\n");
		exit (1);
	}

	//determine which scoket connection this should be (TCP or UDP).
	int addFam;
	int type;
	if (isTCP) {
		addFam = AF_INET;
		type = SOCK_STREAM;
	} else {
		addFam = PF_INET;
		type = SOCK_DGRAM;
	}

	//create the socket based on flags indicating TCP/UDP
	if ((ProgSocket = socket (addFam, type, 0)) == -1) {
		perror ("Cannot create socket");
		exit (1);
	}

}


void CloseConnection () {
	Connection_Setup = FALSE;
	closesocket (ProgSocket);
	WSACleanup ();
}