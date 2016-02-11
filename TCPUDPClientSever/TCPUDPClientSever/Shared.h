#ifndef SHARED_H
#define SHARED_H

#define DEFAULT_PORT_NUMBER		4500
#define BUFFER_SIZE				512

#include <windows.h>
#include <Windowsx.h>
#include "SetupWindow.h"
#include "MainWindow.h"

#include "Connection.h"
#include "Application.h"

//handle of the main GUI window.
extern HWND hMain;

//handle of the startup window
extern HWND hStart;

extern HINSTANCE progInst;

extern int commandID;

extern int port;



#endif