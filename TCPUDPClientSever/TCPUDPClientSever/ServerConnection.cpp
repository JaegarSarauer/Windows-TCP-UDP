#include "ServerConnection.h"

struct sockaddr_in server;


DWORD WINAPI RunTCPServer (LPVOID lpParam) {
	ServiceThreadParams *param = (ServiceThreadParams *)lpParam;
	struct sockaddr_in client;
	SOCKET ReadSocket;
	int dataRead, dataToRead;
	char *inputBuffer = (char *)malloc (packet_size * sizeof (char));
	bool transmissionEnded = FALSE;


	//set socket to non-blocking
	u_long iMode = 1;
	ioctlsocket (ProgSocket, FIONBIO, &iMode);


	// Listen for connections
	// queue up to 5 connect requests
	listen (param->ConnectionSocket, 5);

	while (Connection_Setup) {
		int client_len = sizeof (client);
		if ((ReadSocket = accept (param->ConnectionSocket, (struct sockaddr *)&client, &client_len)) != -1) {
			dataToRead = packet_size;
			transmissionEnded = FALSE;
			do {
				while ((dataRead = recv (ReadSocket, inputBuffer, dataToRead, 0)) < packet_size && !transmissionEnded) {
					if (dataRead == 0)
						transmissionEnded = TRUE;
					if (dataRead != -1 && WSAGetLastError () != WSAEWOULDBLOCK) {
						//if (*inputBuffer == (char)4)
						inputBuffer += dataRead;
						dataToRead -= dataRead;
						if (dataRead == 0)
							break;
					}
				}
				if (!transmissionEnded)
					addLine (std::string (inputBuffer));
				inputBuffer = (char *)malloc (packet_size * sizeof (char));
			} while (!transmissionEnded);
			closesocket (ReadSocket);
		}
	}
	return 0;
}

DWORD WINAPI RunUDPServer (LPVOID lpParam) {
	ServiceThreadParams *param = (ServiceThreadParams *)lpParam;
	struct sockaddr_in client;
	int dataRead;
	char *inputBuffer = (char *)malloc (packet_size * sizeof (char));
	bool transmissionEnded = FALSE;


	//set socket to non-blocking
	//u_long iMode = 1;
	//ioctlsocket (ProgSocket, FIONBIO, &iMode);

	while (Connection_Setup) {
		int client_len = sizeof (client);
		transmissionEnded = FALSE;
		if ((dataRead = recvfrom (ProgSocket, inputBuffer, packet_size, 0, (struct sockaddr *)&client, &client_len))) {
			if (dataRead != -1 && WSAGetLastError () != WSAEWOULDBLOCK) {
				addLine (std::string (inputBuffer));
				inputBuffer = (char *)malloc (packet_size * sizeof (char));
			} else {
				transmissionEnded = TRUE;
			}
		}
	}
	
	return 0;
}


void SetupAsServer () {
	Button_SetText (GetDlgItem (hMain, IDC_ACTION_BUTTON), "Allow Connections");
	EnableWindow (GetDlgItem (hMain, IDC_IP), FALSE);
}

void ConnectServer () {

	CreateSocket ();

	// Initialize and set up the address structure
	memset ((char *)&server, 0, sizeof (struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons (port);
	server.sin_addr.s_addr = htonl (INADDR_ANY); // Accept connections from any client

	/*if (bind (ProgSocket, (struct sockaddr *)&server, sizeof (server)) == SOCKET_ERROR) {
		CloseConnection ();
		MessageBox (hwnd, "Unable to bind to socket", "Unable to Bind", MB_OK);
		return;
	}*/



	MessageBox (hwnd, "Ready for connections.", "Success!", MB_OK);
	Connection_Setup = TRUE;
}

void StartServerThread () {
	ServiceThreadParams *data = (ServiceThreadParams *)malloc (sizeof (ServiceThreadParams));
	data->ConnectionSocket = ProgSocket;
	HANDLE ServiceThread;

	switch (proto_type) {
		case PROTOCOL_TCP:
			ServiceThread = CreateThread (NULL, 0, RunTCPServer, data, 0, NULL);
			break;
		case PROTOCOL_UDP:
			ServiceThread = CreateThread (NULL, 0, RunUDPServer, data, 0, NULL);
			break;
		default:
			break;
	}
}