#ifndef APPLICATION_H
#define APPLICATION_H

#define MAX_DATA		2147483646

#include "Shared.h"
#include "SetupWindow.h"
#include <stdio.h>
#include <regex>


INT_PTR CALLBACK WndProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow);

void showDataChecked();
void falseDataChecked();
void updateStatsWindow(struct Statistics *s);


#endif