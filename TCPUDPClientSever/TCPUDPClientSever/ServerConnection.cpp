#include "ServerConnection.h"

struct sockaddr_in server;

DWORD WINAPI RunTCPServer(LPVOID lpParam) {
	SYSTEMTIME sysStart, sysEnd;
	ServiceThreadParams *param = (ServiceThreadParams *)lpParam;
	struct sockaddr_in client;
	SOCKET ReadSocket;
	int dataRead, dataToRead;
	char *inputBuffer = (char *)malloc(MAX_BUFFER * sizeof(char));
	bool transmissionEnded = FALSE;
	bool firstRead = TRUE;


	//set socket to non-blocking
	u_long iMode = 1;
	ioctlsocket(ProgSocket, FIONBIO, &iMode);


	// Listen for connections
	// queue up to 5 connect requests
	listen(param->ConnectionSocket, 5);

	while (Connection_Setup) {
		int client_len = sizeof(client);
		if ((ReadSocket = accept(param->ConnectionSocket, (struct sockaddr *)&client, &client_len)) != -1) {
			dataToRead = packet_size;
			transmissionEnded = FALSE;
			do {
				while ((dataRead = recv(ReadSocket, inputBuffer, dataToRead, 0)) < dataToRead && !transmissionEnded) {
					if (dataRead == 0)
						transmissionEnded = TRUE;
					if (dataRead != -1 && WSAGetLastError() != WSAEWOULDBLOCK) {
						//if (*inputBuffer == (char)4)
						inputBuffer += dataRead;
						dataToRead -= dataRead;
					}
				}
				if (!transmissionEnded) {
					if (firstRead) {
						firstRead = FALSE;
						GetSystemTime(&sysStart);
					}
					if (show_data)
						addLine(std::string(inputBuffer));
					stats->packets++;
					updateStatsWindow(stats);
					dataToRead = packet_size;
					GetSystemTime(&sysEnd);
					stats->time = packetDelay(sysStart, sysEnd);
					inputBuffer = (char *)malloc(MAX_BUFFER * sizeof(char));
				}
			} while (!transmissionEnded);
			firstRead = TRUE;
			closesocket(ReadSocket);
			GetSystemTime(&sysEnd);
			stats->time = packetDelay(sysStart, sysEnd);
		}
	}

	return 0;
}

DWORD WINAPI RunUDPServer(LPVOID lpParam) {
	SYSTEMTIME sysStart, sysEnd;
	ServiceThreadParams *param = (ServiceThreadParams *)lpParam;
	struct sockaddr_in client;
	int dataRead;
	char *inputBuffer = (char *)malloc(MAX_BUFFER * sizeof(char));
	bool transmissionEnded = FALSE;
	int misRead = 4000000;

	bool hasReadData = FALSE;

	//set socket to non-blocking
	u_long iMode = 1;
	ioctlsocket(ProgSocket, FIONBIO, &iMode);

	while (Connection_Setup) {
		int client_len = sizeof(client);
		transmissionEnded = FALSE;
		if ((dataRead = recvfrom(ProgSocket, inputBuffer, packet_size, 0, (struct sockaddr *)&client, &client_len))) {
			if (dataRead != -1 && WSAGetLastError() != WSAEWOULDBLOCK) {
				misRead = 4000000;
				if (show_data) {
					inputBuffer[dataRead] = '\0';
					addLine(std::string(inputBuffer));
				}
				if (!hasReadData) {
					GetSystemTime(&sysStart);
					hasReadData = TRUE;
				}
				stats->packets++;
				updateStatsWindow(stats);
				inputBuffer = (char *)malloc(MAX_BUFFER * sizeof(char));
				GetSystemTime(&sysEnd);
				stats->time = packetDelay(sysStart, sysEnd);
			} else {
				if (--misRead <= 0)
					hasReadData = FALSE;
			}
		}
	}

	return 0;
}


void SetupAsServer() {
	Button_SetText(GetDlgItem(hMain, IDC_ACTION_BUTTON), "Allow Connections");
	EnableWindow(GetDlgItem(hMain, IDC_IP), FALSE);
}

void ConnectServer() {

	CreateSocket();

	// Initialize and set up the address structure
	memset((char *)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client


	if (bind(ProgSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
		CloseConnection();
		MessageBox(hwnd, "Unable to bind to socket", "Unable to Bind", MB_OK);
		return;
	}



	MessageBox(hwnd, "Ready for connections.", "Success!", MB_OK);
	Connection_Setup = TRUE;
}

void StartServerThread() {
	ServiceThreadParams *data = (ServiceThreadParams *)malloc(sizeof(ServiceThreadParams));
	data->ConnectionSocket = ProgSocket;
	HANDLE ServiceThread;

	switch (proto_type) {
	case PROTOCOL_TCP:
		ServiceThread = CreateThread(NULL, 0, RunTCPServer, data, 0, NULL);
		break;
	case PROTOCOL_UDP:
		ServiceThread = CreateThread(NULL, 0, RunUDPServer, data, 0, NULL);
		break;
	default:
		break;
	}
}