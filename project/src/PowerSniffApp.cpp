///////////////////////////////////////////////////////////
//PowerSniffApp.cpp -  Definitions for the CPowerSniffApp class

#include "stdafx.h"
#include "resource.h"
#include "PowerSniffApp.h"
#include "../scintilla/Scintilla.h"
#include "../other/ConfigWrap.h"
#include "../other/PluginTCC.h"
#include "../other/PluginPython.h"


CPowerSniffApp::CPowerSniffApp(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
}

CPowerSniffApp::~CPowerSniffApp()
{
	Scintilla_ReleaseResources();
}

BOOL CPowerSniffApp::InitInstance()
{
	// 扫描是否支持libtcc和python的插件
	CheckLibTcc();
	CheckLibPython();

	// scintilla文本编辑器：注意需要反初始化
	Scintilla_RegisterClasses(m_hInstance);

	//Create the Frame Window
	if (m_Frame.Create() == 0) {
		// We get here if the Frame creation fails

		::MessageBox(NULL, L"Failed to create Frame window", L"ERROR", MB_ICONERROR);
		return FALSE; // returning FALSE ends the application
	}

	return TRUE;
}
