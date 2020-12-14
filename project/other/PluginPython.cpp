#include "stdafx.h"
#include "PluginPython.h"

Py_Initialize1		Py_Initialize		= NULL;
Py_Finalize1		Py_Finalize		= NULL;
Py_SetPythonHome1	Py_SetPythonHome = NULL;

static int python_test();
void CheckLibPython()
{
	TCHAR findpath[256] = _T("");
	GetModuleFileName(NULL, findpath, MAX_PATH);
	TCHAR *p = wcsrchr(findpath, _T('\\'));
	*p = _T('\0');
	wcscat(findpath, _T("\\3rd\\python37.dll"));

	HMODULE m_hModule = ::LoadLibrary(findpath);
	if (NULL == m_hModule)
		return;

	Py_Initialize = (Py_Initialize1)GetProcAddress(m_hModule, "Py_Initialize");
	Py_Finalize = (Py_Finalize1)GetProcAddress(m_hModule, "Py_Finalize");
	Py_SetPythonHome = (Py_SetPythonHome1)GetProcAddress(m_hModule, "Py_SetPythonHome");

	python_test();
}

int python_test()
{
	TCHAR findpath[256] = _T("");
	GetModuleFileName(NULL, findpath, MAX_PATH);
	TCHAR *p = wcsrchr(findpath, _T('\\'));
	*p = _T('\0');
	wcscat(findpath, _T("\\3rd\\python"));
	Py_SetPythonHome(findpath);			// 重要，设置python文件夹，里面有lib文件夹是python35.dll需要的
	Py_Initialize();
	Py_Finalize();
	return 0;
}
