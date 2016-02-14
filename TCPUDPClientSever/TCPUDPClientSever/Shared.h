#ifndef SHARED_H
#define SHARED_H

#define DEFAULT_PORT_NUMBER		4500
#define DEFAULT_BUFFER_SIZE		512

/*-------------------------------
*	File browser window variables
-------------------------------*/
#define SAVE_BROWSER	2530
#define OPEN_BROWSER	2531

#pragma comment(lib, "Ws2_32.lib")

#include <windows.h>
#include <Windowsx.h>
#include <string>
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

extern bool isClient;
extern bool isTCP;

extern HANDLE		hf;              // file handle
extern OPENFILENAME ofn;			 // common dialog box structure

void StartServerThread ();
void CreateSocket ();

// owner window
extern HWND hwnd;

struct ServiceThreadParams {
	SOCKET ConnectionSocket;
};


void ConnectClient ();
void SetupAsServer (bool isTCP);
void SetupAsClient (bool isTCP);
DWORD WINAPI RunServer (LPVOID lpParam);
void RunClient ();
void ConnectServer ();
void addLine (std::string line);
int getLines ();
std::string getLine (int line);
void clearBox ();

#endif