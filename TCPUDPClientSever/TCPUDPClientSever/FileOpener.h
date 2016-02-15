#ifndef OPENFILE_H
#define OPENFILE_H
#define MAX_FILE	1000000
#define READ_CHUNK	4096

#include "Shared.h"
#include <commctrl.h>
#include <iostream>
#include <fstream>


/*------------------------------------------------------------------------------------------------------------------


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
void initFileOpener ();

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
void setFileOpenerFlags (int browserAction);

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
DWORD WINAPI createFileReader (LPVOID lpParam);

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
DWORD WINAPI createFileWriter (LPVOID lpParam);

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
//void loadFileToView (LPSTR file);
DWORD WINAPI loadFileToView(LPVOID lpParam);

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
void saveFileToComputer (LPSTR file);


#endif
