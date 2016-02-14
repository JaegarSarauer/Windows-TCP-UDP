#include "FileOpener.h"


//file opener vars
//file opener dialog structure.
OPENFILENAME ofn;

//handle of the file opener.
HANDLE hf;

//char* for file path.
char szFile[260];

std::regex addNewLine ("(?!\r)\n");


/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		createFileReader
--
--	DATE:			November 23rd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		DWORD WINAPI createFileReader(LPVOID lpParam);
--						LPVOID lpParam = Required by the CreateThread function to be able to multithread this action.
--
--	RETURNS:		DWORD WINAPI
--
--	NOTES: 			This function creates a file on the handle for the file opener. The file opener is a window which allows
--					the user to browse through their computer. This function is specific to opening the file browser to open
--					files, rather than save them.
--
--					Because there is only one window that needs to deal with the loading of files, this is specifically designed
--					for the left side, "local" window.
--
--					The window is built on the OPENFILENAME struct, which details information about constraints and features
--					the window will offer the user. More details on the OPENFILENAME struct and how it is setup is available
--					at the description for initFileOpener function.
-------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI createFileReader (LPVOID lpParam) {
	hf = CreateFile (ofn.lpstrFile,
		GENERIC_READ,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)hMain);
	CloseHandle(hf);
	return 0;
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		createFileWriter
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		DWORD WINAPI createFileReader(LPVOID lpParam);
--						LPVOID lpParam = Required by the CreateThread function to be able to multithread this action.
--
--	RETURNS:		DWORD WINAPI
--
--	NOTES:			This function creates a file on the handle for the file opener. The file opener is a window which
--					allows the user to browse through their computer. This function is specific to opening the file
--					browser to save	files, rather than open them.
--
--					Both windows of this application require the need to use the save window.
--
--					The window is built on the OPENFILENAME struct, which details information about constraints and features
--					the window will offer the user. More details on the OPENFILENAME struct and how it is setup is available
--					at the description for initFileOpener function.
-------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI createFileWriter (LPVOID lpParam) {
	hf = CreateFile (ofn.lpstrFile,
		GENERIC_WRITE,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)hMain);
	CloseHandle(hf);
	return 0;
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		loadFileToView
--
--	DATE:			November 23rd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void loadFileToView(const HWND *box, LPSTR file);
--						const HWND *box		= The specific HWND this file is being loaded to.
--						LPSTR file			= The file path the file will be read from.
--
--	RETURNS:		VOID
--
--	NOTES:			This function uses the constructed file from the createFileReader to load a plain text file
--					into the specified handle, (*box).
--
--					The function parses the file line by line, and sends each parsed line as a line into the specified
--					handle (normally an Edit Box). In order for this to populate the correctly, the function must find
--					the end of the file each time it appends a line to the handle, to prevent file overwrites.
-------------------------------------------------------------------------------------------------------------------*/
void loadFileToView (LPSTR file) {
	int idx;
	std::string tmp;
	HWND box = GetDlgItem (hMain, IDC_DATA_BOX);

	std::ifstream inputF (file);
	clearBox ();
	SendMessage (box, EM_SETREADONLY, (LPARAM)FALSE, NULL);
	while (getline (inputF, tmp)) {
		tmp += "\n";
		idx = GetWindowTextLength (box);
		SendMessage (box, EM_SETSEL, (LPARAM)idx, (LPARAM)idx);
		SendMessage (box, EM_REPLACESEL, 0, (LPARAM)(tmp.c_str ()));
	}

	inputF.close ();
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		saveFileToComputer
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void saveFileToComputer(const HWND *box, LPSTR file);
--						const HWND *box		= The HWND to target to save the data from.
--						LPSTR file			= The file path to save a new file to.
--
--	RETURNS:		VOID
--
--	NOTES:			This function requires to know which Edit Control it will be getting its data from. When this
--					function is called, the file to write to is opened and line-by-line, the data is sent from the
--					specified edit control into the new, or previously created file (overwriting previous data).
--
-------------------------------------------------------------------------------------------------------------------*/
//loads a file (from a file path) into the left side window, line by line.
void saveFileToComputer (LPSTR file) {
	int idx;
	size_t onLine = 0,
		totalLines = getLines ();
	std::string tmp;
	std::ofstream outputF (file);
	for (; onLine < totalLines; onLine++) {
		outputF << getLine (onLine);
	}
	outputF.close ();

}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		initFileOpener
--
--	DATE:			November 23rd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void initFileOpener();
--
--	RETURNS:		VOID
--
--	NOTES:			This function initializes the default paramters for the file opener. This function only needs
--					to be called once to set it up. At the end of the function, setFileOpenerFlags is called to setup
--					default flags, however these would be reassigned each time a file opener for save or open is called.
--
-------------------------------------------------------------------------------------------------------------------*/
//initializes the parameters for the file opener. This only needs to be called once to set it up.
void initFileOpener () {
	ZeroMemory (&ofn, sizeof (ofn));
	ofn.lStructSize = sizeof (ofn);
	ofn.hwndOwner = hMain;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof (szFile);
	ofn.lpstrFilter = "Text Files\0*.txt\0";
	ofn.lpstrDefExt = "txt";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	setFileOpenerFlags (OPEN_BROWSER);
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		setFileOpenerFlags
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1 = The origional code was used in the initFileOpener.
--					Jaegar Sarauer - Version 2 = The code was split into this function so the flags may be changed, and
--												 still use the same struct for the file opener.
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void setFileOpenerFlags(int browserAction)
--						int browserAction		= Is the file browser being set for reading or writing.
--
--	RETURNS:		VOID
--
--	NOTES:			This function sets the correct flags for what type of file processing it is doing. (reading or
--					saving a file).
--
-------------------------------------------------------------------------------------------------------------------*/
void setFileOpenerFlags (int browserAction) {
	ofn.Flags = 0;
	switch (browserAction) {
		case SAVE_BROWSER:
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING;
			break;
		case OPEN_BROWSER:
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_HIDEREADONLY;
			break;
	}
}
