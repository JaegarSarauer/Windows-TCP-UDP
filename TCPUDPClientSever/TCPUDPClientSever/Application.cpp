#define STRICT
#include "Application.h"

// owner window
HWND hwnd;
MSG Msg;

//handle of the file opener.
extern HANDLE hf;

//handle of the main GUI window.
HWND hMain;

//handle of the startup window
HWND hStart;

HINSTANCE progInst;

int commandID;

int port = DEFAULT_PORT_NUMBER;
int packet_size = DEFAULT_BUFFER_SIZE;


int prog_type;
int proto_type;

bool Connection_Setup = FALSE;

bool show_data; 
bool false_data;
int packets_to_send;

struct Statistics *stats;


int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow) {

	progInst = hInst;
	commandID = nCmdShow;

	// State - Build Window
	hStart = CreateDialogParam (hInst, MAKEINTRESOURCE (IDD_SETUP), 0, WndProc, 0);
	ShowWindow (hStart, nCmdShow);


	SendMessage (GetDlgItem (hStart, IDC_CLIENT_RADIO), BM_SETCHECK, BST_CHECKED, NULL);
	SendMessage (GetDlgItem (hStart, IDC_TCP_RADIO), BM_SETCHECK, BST_CHECKED, NULL);

	stats = new Statistics;
	stats->status = "Waiting for Action";

	stats->fileLoad = 0;
	stats->fileLoadTotal = 0;
	stats->packets = 0;
	stats->time = 0;

	initFileOpener ();

	while (GetMessage (&Msg, NULL, 0, 0)) {
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}

	return Msg.wParam;
}





int IsProgramClient () {
	if (SendMessage (GetDlgItem (hStart, IDC_CLIENT_RADIO), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
		return PROGRAM_CLIENT;
	else
		return PROGRAM_SERVER;
}

int IsProtocolTCP () {
	if (SendMessage (GetDlgItem (hStart, IDC_TCP_RADIO), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
		return PROTOCOL_TCP;
	else
		return PROTOCOL_UDP;
}


const int tmpSize = 8;
void StartMainWindow () {
	char tmp[tmpSize];

	hMain = CreateDialogParam (progInst, MAKEINTRESOURCE (IDD_MAIN_WINDOW), 0, WndProc, 0);
	ShowWindow (hMain, commandID);

	//set max sizes for the UI elements.
	SendMessage (GetDlgItem (hMain, IDC_DATA_BOX), EM_SETLIMITTEXT, (LPARAM)MAX_DATA, NULL);
	SendMessage (GetDlgItem (hMain, IDC_PORT_EDIT), EM_SETLIMITTEXT, (LPARAM)tmpSize, NULL);
	SendMessage (GetDlgItem (hMain, IDC_PACKET_SIZE), EM_SETLIMITTEXT, (LPARAM)tmpSize, NULL);
	SendMessage (GetDlgItem (hMain, IDC_SHOW_DATA), BM_SETCHECK, BST_CHECKED, 0);
	showDataChecked ();

	//set a default port number in GUI
	sprintf_s (tmp, "%d", port);
	Edit_SetText (GetDlgItem (hMain, IDC_PORT_EDIT), tmp);

	//set a default packet size in GUI
	sprintf_s (tmp, "%d", packet_size);
	Edit_SetText (GetDlgItem (hMain, IDC_PACKET_SIZE), tmp);


	updateStatsWindow (stats);

	switch (prog_type) {
		case PROGRAM_CLIENT:
			SetupAsClient ();
			break;
		case PROGRAM_SERVER:
			SetupAsServer ();
			break;
		default:
			break;
	}
}

void GetConnectionParameters () {
	char tmp[tmpSize];
	GetWindowText (GetDlgItem (hMain, IDC_PORT_EDIT), tmp, tmpSize);
	port = atoi (tmp);

	GetWindowText (GetDlgItem (hMain, IDC_PACKET_SIZE), tmp, tmpSize);
	packet_size = atoi (tmp);
}

void updateStatsWindow (struct Statistics *s) {
	char tmp[1024];
	switch (prog_type) {
		case PROGRAM_CLIENT:
			sprintf_s (tmp, "Status: %s\r\nFile Loaded: %d/%d\r\nPackets Sent: %d\r\nTransfer Time: %ld", s->status, s->fileLoad, s->fileLoadTotal, s->packets, s->time);
			break;
		case PROGRAM_SERVER:
			sprintf_s (tmp, "Status: %s\r\nPackets Recieved: %d\r\nTransfer Time: %ld", s->status, s->packets, s->time);
			break;
	}
	Edit_SetText (GetDlgItem (hMain, IDC_STATS_BOX), tmp);
}

INT_PTR CALLBACK WndProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_COMMAND:
			switch (LOWORD (wParam)) {
				case IDC_SHOW_DATA:
					switch (HIWORD (wParam)) {
						case BN_CLICKED:
							showDataChecked ();
							break;
					}
					break;
				case IDC_FALSE_DATA:
					switch (HIWORD(wParam)) {
					case BN_CLICKED:
						falseDataChecked();
						break;
					}
					break;
				case IDC_CLEAR_BUTTON:
					clearBox ();
					break;
				case IDC_ACTION_BUTTON:
					GetConnectionParameters ();
					if (packet_size < 4) {
						MessageBox (hwnd, "Packet size must be at least 4 chars.", "Larger Packets?!", MB_OK);
						break;
					}
					stats->status = "Connecting";
					updateStatsWindow (stats);
					switch (prog_type) {
						case PROGRAM_CLIENT:
							ConnectClient ();
							if (Connection_Setup) {
								if (false_data)
									getPacketsToSend();
								stats->status = "Sending Data...";
								updateStatsWindow (stats);
								switch (proto_type) {
									case PROTOCOL_TCP:
										RunTCPClient ();
										break;
									case PROTOCOL_UDP:
										RunUDPClient ();
										break;
									default:
										break;
								}
							}
							stats->status = "Sending Complete!";
							updateStatsWindow (stats);
							CloseConnection ();
							break;
						case PROGRAM_SERVER:
							CloseConnection();
							ConnectServer ();
							if (Connection_Setup) {
								stats->status = "Waiting for Data...";
								updateStatsWindow (stats);
								StartServerThread ();
							}
							break;
						default:
							break;
					}
					break;
				case IDC_OPEN_BUTTON:
					setFileOpenerFlags (OPEN_BROWSER);
					if (GetOpenFileName (&ofn) == TRUE) {
						CreateThread(NULL, 0, createFileReader, NULL, 0, NULL);
						CreateThread(NULL, 0, loadFileToView, ofn.lpstrFile, 0, NULL);
						//loadFileToView (ofn.lpstrFile);
					}
					break;
				case IDC_SAVE_BUTTON:
					setFileOpenerFlags(SAVE_BROWSER);
					if (GetOpenFileName(&ofn) == TRUE) {
						CreateThread(NULL, 0, createFileWriter, NULL, 0, NULL);
						saveFileToComputer (ofn.lpstrFile);
					}
					break;
				case IDC_EXIT_BUTTON:
				case IDC_EXIT_START:
					CloseConnection ();
					PostQuitMessage (0);
					break;
				case IDC_START_BUTTON:
					prog_type = IsProgramClient ();
					proto_type = IsProtocolTCP ();
					StartMainWindow ();
					DestroyWindow (hStart);
					break;
			}
			break;

		case WM_CLOSE:	// Terminate program
			CloseConnection ();
			PostQuitMessage (0);
			break;
		default:
			return false;
	}
	return 0;
}


/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		addLine
--
--	DATE:			December 24th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1 = Default sending lines to control, tested with the two Edit controls.
--					Jaegar Sarauer - Version 2 = Added regex parsing to insert new lines (requires \r\n rather than
--												 just \n character.
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void addLine(const HWND *box, std::string line)
--						const HWND *box		=
--						std::string line	=
--
--	RETURNS:		VOID
--
--	NOTES:			This function adds a single string to the end of the specified edit control. This line will be
--					ended with a new line character, requiring that you will send what you expect to be your entire
--					line at once. (You may not parse to a previously added line.)
--
-------------------------------------------------------------------------------------------------------------------*/
//adds a line to whatever handle you would like. It is expected to pass in a file holder as the handle.
void addLine (std::string line) {
	int idx;
	if (line == "")
		return;
	HWND box = GetDlgItem (hMain, IDC_DATA_BOX);

	idx = GetWindowTextLength (box);
	std::regex e ("(?!\r)\n");
	std::string tmp = std::regex_replace (line, e, "\r\n");
	SendMessage (box, EM_SETSEL, (LPARAM)idx, (LPARAM)idx);
	SendMessage (box, EM_REPLACESEL, 0, (LPARAM)tmp.c_str ());
}

/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		getLines
--
--	DATE:			December 23rd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		int getLines(const HWND *box)
--						const HWND *box		=
--
--	RETURNS:		int		=		Returns the amount of lines of text the specific UI element has.
--
--	NOTES:			This function targets the specified Edit Control, and returns the amount of lines there are of text
--					in the control.
--
-------------------------------------------------------------------------------------------------------------------*/
//returns the amount of lines in the specific handle.
int getLines () {
	HWND box = GetDlgItem (hMain, IDC_DATA_BOX);
	return SendMessage (box, EM_GETLINECOUNT, NULL, NULL);
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		getLine
--
--	DATE:			 , 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		std::string getLine(const HWND *box, int line)
--						const HWND *box		= The HWND to which Edit Box the line is being retrieved from.
--						int line			= The line index which is being retrived from the specified edit box.
--
--	RETURNS:		std::string				= The string retrieved from the specified line.
--
--	NOTES:			This function gets a single line, specified from the int line parameter, and returns it.
--
-------------------------------------------------------------------------------------------------------------------*/
//this function targets a handle and gets the line as string at an index you choose.
std::string getLine (int line) {
	size_t len;
	TCHAR *tmp;
	HWND box = GetDlgItem (hMain, IDC_DATA_BOX);

	len = SendMessage (box, EM_LINELENGTH, SendMessage (box, EM_LINEINDEX, line, 0), NULL);
	tmp = new TCHAR[len];
	*tmp = 0;
	SendMessage (box, EM_GETLINE, line, (LPARAM)tmp);
	tmp[len] = '\0';
	return tmp;
}


/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		clearBox
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void clearBox(const HWND *box);
--						const HWND *box		= The specific HWND that the text is being set to empty.
--
--	RETURNS:		VOID
--
--	NOTES:			This function takes in a handle to a UI element, and sets it's text to be an empty string. It
--					is inteded to be called when the user clicks on one of the clear buttons for the Edit Boxes.
-------------------------------------------------------------------------------------------------------------------*/
void clearBox () {
	SetWindowText (GetDlgItem (hMain, IDC_DATA_BOX), "");
	stats->status = "Waiting for Action";
	stats->fileLoad = 0;
	stats->fileLoadTotal = 0;
	stats->packets = 0;
	stats->time = 0;
	updateStatsWindow(stats);
}


/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		checkBoxChecked
--
--	DATE:			November 30th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void checkBoxChecked();
--
--	RETURNS:		VOID
--
--	NOTES: 			This function updates the priority from the check
--					box on the GUI, next to the "Send" button. It is intended
--					to be called when the user clicks on the checkbox,
--					allowing the status to be updated.
-------------------------------------------------------------------------------------------------------------------*/
void showDataChecked () {
	show_data = SendMessage (GetDlgItem (hMain, IDC_SHOW_DATA), BM_GETCHECK, 0, 0);
	ShowWindow (GetDlgItem (hMain, IDC_DATA_BOX), show_data);
}

/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		checkBoxChecked
--
--	DATE:			November 30th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void checkBoxChecked();
--
--	RETURNS:		VOID
--
--	NOTES: 			This function updates the priority from the check
--					box on the GUI, next to the "Send" button. It is intended
--					to be called when the user clicks on the checkbox,
--					allowing the status to be updated.
-------------------------------------------------------------------------------------------------------------------*/
void falseDataChecked() {
	false_data = SendMessage(GetDlgItem(hMain, IDC_FALSE_DATA), BM_GETCHECK, 0, 0);
}


void getPacketsToSend() {
	char tmp[8];
	GetWindowText(GetDlgItem(hMain, IDC_PACKET_COUNT), tmp, tmpSize);
	packets_to_send = atoi(tmp);
}