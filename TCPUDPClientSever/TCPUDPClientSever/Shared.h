#ifndef SHARED_H
#define SHARED_H

#define DEFAULT_PORT_NUMBER		4500
#define DEFAULT_BUFFER_SIZE		512

#define PROTOCOL_TCP			455
#define PROTOCOL_UDP			456

#define PROGRAM_CLIENT			545
#define PROGRAM_SERVER			546

/*-------------------------------
*	File browser window variables
-------------------------------*/
#define SAVE_BROWSER	2530
#define OPEN_BROWSER	2531

#define LOCAL_IP "127.0.0.1"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)
#pragma comment(lib, "Ws2_32.lib")

//#include <windows.h>
#include <Windowsx.h>
#include <string>
#include <WinSock2.h>
#include "SetupWindow.h"
#include "MainWindow.h"

#include "Connection.h"
#include "ClientConnection.h"
#include "ServerConnection.h"
#include "Application.h"
#include "Packetizer.h"
#include "FileOpener.h"



//handle of the main GUI window.
extern HWND hMain;

//handle of the startup window
extern HWND hStart;

extern HINSTANCE progInst;

extern int commandID;

extern int port;
extern int packet_size;

extern bool Connection_Setup;

extern SOCKET ProgSocket;

extern WSADATA WSAData;

extern struct sockaddr_in server;

extern int prog_type;
extern int proto_type;
extern bool show_data;
extern bool false_data;
extern int packets_to_send;

extern HANDLE		hf;              // file handle
extern OPENFILENAME ofn;			 // common dialog box structure

void StartServerThread ();
void CreateSocket ();

// owner window
extern HWND hwnd;

struct ServiceThreadParams {
	SOCKET ConnectionSocket;
};

typedef struct Statistics {
	char* status;
	int fileLoad;
	int fileLoadTotal;
	int packets;
	long time;
} Stats;

extern struct Statistics *stats;

void ConnectClient ();
void SetupAsServer ();
void SetupAsClient ();
DWORD WINAPI RunTCPServer (LPVOID lpParam);
DWORD WINAPI RunUDPServer (LPVOID lpParam);
void RunTCPClient ();
void RunUDPClient ();
void ConnectServer ();
void addLine (std::string line);
int getLines ();
std::string getLine (int line);
void clearBox ();
void GetConnectionParameters ();
void CloseConnection ();

#endif