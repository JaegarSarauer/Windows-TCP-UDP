#ifndef APPLICATION_H
#define APPLICATION_H

#include "Shared.h"
#include "SetupWindow.h"
#include <stdio.h>


INT_PTR CALLBACK WndProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow);



#endif