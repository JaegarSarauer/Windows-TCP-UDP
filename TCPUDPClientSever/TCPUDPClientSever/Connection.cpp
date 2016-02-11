#include "Connection.h"


char IPAddress[16] = "";
SOCKET ProgSocket;

WSADATA WSAData;

struct sockaddr_in server;
struct hostent	*hp;

void SetupAsClient (bool isTCP) {
	Button_SetText (GetDlgItem (hMain, IDC_ACTION_BUTTON), "Send");
}

void SetupAsServer (bool isTCP) {
	Button_SetText (GetDlgItem (hMain, IDC_ACTION_BUTTON), "Allow Connections");
	EnableWindow (GetDlgItem (hMain, IDC_IP), FALSE);
}

void RunClient () {
	memset ((char *)&server, 0, sizeof (struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons (port);
	if ((hp = gethostbyname (IPAddress)) == NULL) {
		fprintf (stderr, "Unknown server address\n");
		exit (1);
	}

	// Copy the server address
	memcpy ((char *)&server.sin_addr, hp->h_addr, hp->h_length);

	// Connecting to the server
	if (connect (ProgSocket, (struct sockaddr *)&server, sizeof (server)) == -1) {
		fprintf (stderr, "Can't connect to server\n");
		perror ("connect");
		exit (1);
	}

				 // Transmit data through the socket
	send (ProgSocket, "nice one", BUFFER_SIZE, 0);

	closesocket (ProgSocket);
	WSACleanup ();
	exit (0);
}

void RunServer () {

}

void SetupShared (bool isClient, bool isTCP) {
	//get the port
	char tmp[8];
	Edit_GetText (GetDlgItem (hStart, IDC_PORT_EDIT), tmp, 8);
	port = atoi (tmp);

	//if were a client, get the IP
	if (isClient)
		SendMessage (GetDlgItem (hStart, IDC_IP), IPM_GETADDRESS, NULL, (DWORD)IPAddress);

	//initiate the DLL for connections
	WORD version;
	version = MAKEWORD (2, 2);
	if (WSAStartup (version, &WSAData) != 0) {
		printf ("DLL not found!\n");
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


/*	memset ((char *)&server, 0, sizeof (server));
	server.sin_family = AF_INET;
	server.sin_port = htons (port);
	server.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (sd, (struct sockaddr *)&server, sizeof (server)) == -1) {
		perror ("Can't bind name to socket");
		exit (1);
	}*/
}


