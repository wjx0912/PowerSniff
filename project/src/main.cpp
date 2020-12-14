///////////////////////////////////////
// main.cpp

#include "stdafx.h"
#include "PowerSniffApp.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	try {
		// Start Win32++
		CPowerSniffApp theApp(hInstance);

		// Run the application
		return theApp.Run();
	}

	// catch all CException types
	catch (const CException &e) {
		// Display the exception and quit
		MessageBox(NULL, e.GetText(), AtoT(e.what()), MB_ICONERROR);

		return -1;
	}
}
