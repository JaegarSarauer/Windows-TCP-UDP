#include "ClientConnection.h"


char IPAddress[16] = "";
SOCKET ProgSocket;

WSADATA WSAData;

struct hostent	*hp;


void SetupAsClient () {
	Button_SetText (GetDlgItem (hMain, IDC_ACTION_BUTTON), "Send");
	SendMessage (GetDlgItem (hMain, IDC_DATA_BOX), EM_SETREADONLY, (LPARAM)FALSE, NULL);
}

void ConnectClient() {
	CreateSocket ();

	//get the IP
	GetWindowText (GetDlgItem (hMain, IDC_IP), IPAddress, sizeof (IPAddress));

	//get the port
	char tmp[8];
	Edit_GetText (GetDlgItem (hMain, IDC_PORT_EDIT), tmp, 8);
	port = atoi (tmp);

	memset ((char *)&server, 0, sizeof (struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons (port);
	if ((hp = gethostbyname (IPAddress)) == NULL) {
		fprintf (stderr, "Unknown server address\n");
		exit (1);
	}


	// Copy the server address
	memcpy ((char *)&server.sin_addr, hp->h_addr, hp->h_length);

	/*if (bind (ProgSocket, (struct sockaddr *)&server, sizeof (server)) == -1) {
		CloseConnection ();
		MessageBox (hwnd, "Unable to bind to socket", "Unable to Bind", MB_OK);
		return;
	}*/

	// Connecting to the server
	if (connect (ProgSocket, (struct sockaddr *)&server, sizeof (server)) == -1) {
		CloseConnection ();
		MessageBox (hwnd, "Unable to connect to server!\nCheck Port and IP Address", "Error Connecting!", MB_OK);
		return;
	}

	//set socket to non-blocking
	u_long iMode = 1;
	ioctlsocket (ProgSocket, FIONBIO, &iMode);

	MessageBox (hwnd, "Successful Connection", "Success!", MB_OK);
	Connection_Setup = TRUE;
}

void RunTCPClient () {
	Packetizer packetizer = Packetizer();
	packetizer.appendPackets ();
	while (packetizer.availablePacket()) {
		send (ProgSocket, packetizer.getPacket().c_str(), packet_size, 0);
		packetizer.updatePacketList ();
	}
	closesocket (ProgSocket);
	Connection_Setup = FALSE;
}

void RunUDPClient () {
	Packetizer packetizer = Packetizer();
	int server_len = sizeof (server);
	packetizer.appendPackets ();
	while (packetizer.availablePacket ()) {
		sendto (ProgSocket, packetizer.getPacket ().c_str (), packet_size, 0, (struct sockaddr *)&server, server_len);
		packetizer.updatePacketList ();
	}
	closesocket (ProgSocket);
	Connection_Setup = FALSE;
}






