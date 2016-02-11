#define STRICT
#include "Application.h"


HWND hwnd;              // owner window
MSG Msg;

//handle of the file opener.
HANDLE hf;

//handle of the main GUI window.
HWND hMain;

//handle of the startup window
HWND hStart;

HINSTANCE progInst;

int commandID;

int port = DEFAULT_PORT_NUMBER;

bool isClient;
bool isTCP;


int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow) {

	progInst = hInst;
	commandID = nCmdShow;

	// State - Build Window
	hStart = CreateDialogParam (hInst, MAKEINTRESOURCE (IDD_SETUP), 0, WndProc, 0);
	ShowWindow (hStart, nCmdShow);

	SendMessage (GetDlgItem (hStart, IDC_CLIENT_RADIO), BM_SETCHECK, BST_CHECKED, NULL);
	SendMessage (GetDlgItem (hStart, IDC_TCP_RADIO), BM_SETCHECK, BST_CHECKED, NULL);
	
	while (GetMessage (&Msg, NULL, 0, 0)) {
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}

	return Msg.wParam;
}





bool IsProgramClient () {
	if (SendMessage (GetDlgItem (hStart, IDC_CLIENT_RADIO), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
		return true;
	else
		return false;
}

bool IsProtocolTCP () {
	if (SendMessage (GetDlgItem (hStart, IDC_TCP_RADIO), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
		return true;
	else
		return false;
}


void StartMainWindow (bool isClient, bool isTCP) {

	hMain = CreateDialogParam (progInst, MAKEINTRESOURCE (IDD_MAIN_WINDOW), 0, WndProc, 0);
	ShowWindow (hMain, commandID);

	//set a default port number in GUI
	char tmp[8];
	sprintf_s (tmp, "%d", port);
	Edit_SetText (GetDlgItem (hMain, IDC_PORT_EDIT), tmp);
	
	if (isClient)
		SetupAsClient (isTCP);
	else
		SetupAsServer (isTCP);
}


INT_PTR CALLBACK WndProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_COMMAND:
			switch (LOWORD (wParam)) {
				/*case IDC_PRIORITYCHECK:
					switch (HIWORD (wParam)) {
						case BN_CLICKED:
							checkBoxChecked ();
							break;
					}
					break;*/
				case IDC_ACTION_BUTTON:
					SetupShared (isClient, isTCP);
					break;
				case IDC_EXIT_BUTTON:
				case IDC_EXIT_START:
					PostQuitMessage (0);
					break;
				case IDC_START_BUTTON:
					isClient = IsProgramClient ();
					isTCP = IsProtocolTCP ();
					StartMainWindow (isClient, isTCP);
					DestroyWindow (hStart);
					break;
			}
			break;

		case WM_CLOSE:	// Terminate program
			PostQuitMessage (0);
			break;
		default:
			return false;
	}
	return 0;
}