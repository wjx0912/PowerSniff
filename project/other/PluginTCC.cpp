#include "stdafx.h"
#include "PluginTCC.h"
#include "../src/PowerSniffApp.h"
#include "../src/Mainfrm.h"

int TCC_XTRACE(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char *szBuffer = (char *)malloc(4096);
	vsprintf(szBuffer, format, args);
	va_end(args);

	XTRACE(RGB(0, 0, 0), CAtoW(szBuffer, CP_UTF8));
	free(szBuffer);
	return 0;
}

int TCC_XTRACE_RAW(int color, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char *szBuffer = (char *)malloc(4096);
	vsprintf(szBuffer, format, args);
	va_end(args);

	XTRACE(color, CAtoW(szBuffer, CP_UTF8));
	free(szBuffer);
	return 0;
}

int TCC_DBGVIEW(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char *szBuffer = (char *)malloc(4096);
	vsprintf(szBuffer, format, args);
	va_end(args);

	::OutputDebugString(CAtoW(szBuffer, CP_UTF8));
	free(szBuffer);
	return 0;
}

// 参数：字符串，字体颜色，字体大小
int TCC_PLUGIN_OUTPUT(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char *szBuffer = (char *)malloc(4096);
	vsprintf(szBuffer, format, args);
	va_end(args);
	Win32xx::CString textcontent = CAtoW(szBuffer, CP_UTF8);

	GetSplitApp().GetMainFrame()->m_pDockTextBottomRightPluginOutput->TextAppend(RGB(0x00, 0x0, 0)
		, DEFAULT_RICHEDIT_TEXT_SIZE, (TCHAR *)textcontent.c_str());
	free(szBuffer);
	return 0;
}

int PLUGIN_OUTPUT_CLEAR()
{
	GetSplitApp().GetMainFrame()->m_pDockTextBottomRightPluginOutput->TextClear();
	return 0;
}

int PLUGIN_SUMMARY(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char *szBuffer = (char *)malloc(4096);
	vsprintf(szBuffer, format, args);
	va_end(args);
	Win32xx::CString textcontent = CAtoW(szBuffer, CP_UTF8);

	GetSplitApp().GetMainFrame()->SetCurrentItemSummary((TCHAR *)textcontent.c_str());
	free(szBuffer);
	return 0;
}

tcc_new1				tcc_new			= NULL;
tcc_delete1				tcc_delete		= NULL;
tcc_set_lib_path1			tcc_set_lib_path	= NULL;
tcc_set_error_func1			tcc_set_error_func	= NULL;
tcc_set_options1			tcc_set_options		= NULL;
tcc_add_include_path1			tcc_add_include_path	= NULL;
tcc_add_sysinclude_path1		tcc_add_sysinclude_path	= NULL;
tcc_define_symbol1			tcc_define_symbol	= NULL;
tcc_undefine_symbol1			tcc_undefine_symbol	= NULL;
tcc_add_file1				tcc_add_file		= NULL;
tcc_compile_string1			tcc_compile_string	= NULL;
tcc_set_output_type1			tcc_set_output_type	= NULL;
tcc_add_library_path1			tcc_add_library_path	= NULL;
tcc_add_library1			tcc_add_library		= NULL;
tcc_add_symbol1				tcc_add_symbol		= NULL;
tcc_output_file1			tcc_output_file		= NULL;
tcc_run1				tcc_run			= NULL;
tcc_relocate1				tcc_relocate		= NULL;
tcc_get_symbol1				tcc_get_symbol		= NULL;

int libtcc_test();

void CheckLibTcc()
{
	TCHAR findpath[256] = _T("");
	GetModuleFileName(NULL, findpath, MAX_PATH);
	TCHAR *p = wcsrchr(findpath, _T('\\'));
	*p = _T('\0');
	wcscat(findpath, _T("\\3rd\\libtcc.dll"));

	HMODULE m_hModule = ::LoadLibrary(findpath);
	if (NULL == m_hModule)
		return;

	tcc_new = (tcc_new1)GetProcAddress(m_hModule, "tcc_new");
	tcc_delete = (tcc_delete1)GetProcAddress(m_hModule, "tcc_delete");
	tcc_set_lib_path = (tcc_set_lib_path1)GetProcAddress(m_hModule, "tcc_set_lib_path");
	tcc_set_error_func = (tcc_set_error_func1)GetProcAddress(m_hModule, "tcc_set_error_func");
	tcc_set_options = (tcc_set_options1)GetProcAddress(m_hModule, "tcc_set_options");
	tcc_add_include_path = (tcc_add_include_path1)GetProcAddress(m_hModule, "tcc_add_include_path");
	tcc_add_sysinclude_path = (tcc_add_sysinclude_path1)GetProcAddress(m_hModule, "tcc_add_sysinclude_path");
	tcc_define_symbol = (tcc_define_symbol1)GetProcAddress(m_hModule, "tcc_define_symbol");
	tcc_undefine_symbol = (tcc_undefine_symbol1)GetProcAddress(m_hModule, "tcc_undefine_symbol");
	tcc_add_file = (tcc_add_file1)GetProcAddress(m_hModule, "tcc_add_file");
	tcc_compile_string = (tcc_compile_string1)GetProcAddress(m_hModule, "tcc_compile_string");
	tcc_set_output_type = (tcc_set_output_type1)GetProcAddress(m_hModule, "tcc_set_output_type");
	tcc_add_library_path = (tcc_add_library_path1)GetProcAddress(m_hModule, "tcc_add_library_path");
	tcc_add_library = (tcc_add_library1)GetProcAddress(m_hModule, "tcc_add_library");
	tcc_add_symbol = (tcc_add_symbol1)GetProcAddress(m_hModule, "tcc_add_symbol");
	tcc_output_file = (tcc_output_file1)GetProcAddress(m_hModule, "tcc_output_file");
	tcc_run = (tcc_run1)GetProcAddress(m_hModule, "tcc_run");
	tcc_relocate = (tcc_relocate1)GetProcAddress(m_hModule, "tcc_relocate");
	tcc_get_symbol = (tcc_get_symbol1)GetProcAddress(m_hModule, "tcc_get_symbol");

#ifdef _DEBUG
	libtcc_test();
#endif
}

void TCC_TRACE(const char *output)
{
	XTRACE(CAtoW(output, CP_UTF8));
}

/* this function is called by the generated code */
int add(int a, int b)
{
	return a + b;
}

char my_program[] =
"#include <stdio.h>\n"
"#include <windows.h>\n"
"int fib(int n)\n"
"{\n"
"    if (n <= 2)\n"
"        return 1;\n"
"    else\n"
"        return fib(n-1) + fib(n-2);\n"
"}\n"
"\n"
"int foo(int n, int m)\n"
"{\n"
"trace(\"scrpit call.\n\");	"
"    return n + m;\n"
"}\n";

typedef int(*func1)(int);
typedef int(*func2)(int, int);

int libtcc_test()
{
	TCCState *s = NULL;

	s = tcc_new();
	if (!s) {
		fprintf(stderr, "Could not create tcc state\n");
		exit(1);
	}

	//tcc_set_error_func(s, NULL, ::error_in_tcc);
	/* if tcclib.h and libtcc1.a are not installed, where can we find them */
	TCHAR filepath[256] = _T("");
	GetModuleFileName(NULL, filepath, MAX_PATH);
	TCHAR *p = wcsrchr(filepath, _T('\\'));
	*p = _T('\0');
	wcscat(filepath, _T("\\3rd\\libtcc\\"));
	tcc_set_lib_path(s, __CWtoA(filepath));		// 文件夹里面必须有libtcc使用的lib和include
	int a = tcc_add_library(s, "user32");
	int b = tcc_add_library(s, "gdi32");
	int c = tcc_add_library(s, "msvcrt");
	int d = tcc_add_library(s, "kernel32");
	a = b = c = d = 0;

	/* MUST BE CALLED before any compilation */
	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

	//XTRACE(_T("debug line: libtcc_test()%d\n"), __LINE__);
	if (tcc_compile_string(s, my_program) == -1)
		return 1;

	/* as a test, we add a symbol that the compiled program can use.
	You may also open a dll with tcc_add_dll() and use symbols from that */
	tcc_add_symbol(s, "add", add);
	tcc_add_symbol(s, "trace", TCC_TRACE);

	/* relocate the code */
	if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
		return 1;

	/* get entry symbol */
	func1 func_fib = (func1)tcc_get_symbol(s, "fib");
	if (!func_fib)
		return 1;
	func2 func_foo = (func2)tcc_get_symbol(s, "foo");
	if (!func_foo)
		return 1;

	/* run the code */
	ASSERT(func_fib(5) == 5);
	ASSERT(func_fib(6) == 8);
	ASSERT(func_fib(7) == 13);
	ASSERT(func_fib(8) == 21);
	ASSERT(func_foo(32, 33) == 65);		// 观察xtrace会有输出

	/* delete the state */
	tcc_delete(s);
	return 0;
}

int __read_plugin_content(LPCTSTR filename, char *buf)
{
	// 获取插件路径
	TCHAR filepath[256] = _T("");
	GetModuleFileName(NULL, filepath, MAX_PATH);
	TCHAR *p = wcsrchr(filepath, _T('\\'));
	*p = _T('\0');
	wcscat(filepath, _T("\\plugin\\"));
	wcscat(filepath, filename);

	CFile file;
	file.Open(filepath, OPEN_EXISTING);
	file.Read(buf, 64 * 1024);
	return 0;
	return -1;
}

void CPluginTCC::unload()
{
	if (NULL == tcc_new)
		return;
	lock();
	if (s) {
		tcc_delete(s);
		s = NULL;
	}
	unlock();
}

typedef void(*func_init)(void);
typedef int(*func_common)(const char *, void *, int, int);

static void __error_in_tcc(void *opaque, const char *str)
{
	XTRACE(CAtoW(str, CP_UTF8));
	XTRACE_RAW(COLOR_BLACK, L"\r\n");
}

void CPluginTCC::reload(LPCTSTR filename)
{
	wcscpy(m_plugin_filename, filename);
	if (NULL == tcc_new)
		return;
	unload();

	lock();
	s = tcc_new();
	tcc_set_error_func(s, NULL, ::__error_in_tcc);
	char *buf = (char *)malloc(64 * 1024);
	std::shared_ptr<void> _free_ptr((void *)buf, [](void *p) {
		free(p);
	});
	if (!s || !buf)
		goto err_out;
	memset(buf, 0x0, 64 * 1024);

#if 1
	/* if tcclib.h and libtcc1.a are not installed, where can we find them */
	TCHAR filepath[256] = _T("");
	TCHAR temp1[256] = _T("");
	GetModuleFileName(NULL, filepath, MAX_PATH);
	TCHAR *p = wcsrchr(filepath, _T('\\')); p++; *p = _T('\0');
	SetCurrentDirectory(filepath);
#endif

#if 1
	wcscpy(temp1, filepath);
	wcscat(temp1, _T("3rd\\libtcc\\"));
	tcc_set_lib_path(s, __CWtoA(temp1));		// 文件夹里面必须有libtcc使用的lib和include
#endif

#if 1
	wcscpy(temp1, filepath);
	tcc_add_sysinclude_path(s, __CWtoA(temp1));
	tcc_add_include_path(s, __CWtoA(temp1));
	tcc_add_library_path(s, __CWtoA(temp1));
	tcc_add_library(s, __CWtoA(temp1));

	wcscat(temp1, _T("lib\\"));
	tcc_add_sysinclude_path(s, __CWtoA(temp1));
	tcc_add_include_path(s, __CWtoA(temp1));
	tcc_add_library_path(s, __CWtoA(temp1));
	tcc_add_library(s, __CWtoA(temp1));
#endif

	tcc_add_library(s, "user32");
	tcc_add_library(s, "gdi32");
	tcc_add_library(s, "msvcrt");
	tcc_add_library(s, "kernel32");

	/* MUST BE CALLED before any compilation */
	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

#if 1
	/* as a test, we add a symbol that the compiled program can use.
	You may also open a dll with tcc_add_dll() and use symbols from that */
	tcc_add_symbol(s, "trace", TCC_XTRACE);
	tcc_add_symbol(s, "TRACE", TCC_XTRACE);
	tcc_add_symbol(s, "xtrace", TCC_XTRACE);
	tcc_add_symbol(s, "XTRACE", TCC_XTRACE);
	tcc_add_symbol(s, "trace_raw", TCC_XTRACE_RAW);
	tcc_add_symbol(s, "TRACE_RAW", TCC_XTRACE_RAW);
	tcc_add_symbol(s, "xtrace_raw", TCC_XTRACE_RAW);
	tcc_add_symbol(s, "XTRACE_RAW", TCC_XTRACE_RAW);
	tcc_add_symbol(s, "dbgview", TCC_DBGVIEW);
	tcc_add_symbol(s, "DBGView", TCC_DBGVIEW);
	tcc_add_symbol(s, "plugin_output", TCC_PLUGIN_OUTPUT);
	tcc_add_symbol(s, "PLUGIN_OUTPUT", TCC_PLUGIN_OUTPUT);
	tcc_add_symbol(s, "plugin_output_clear", PLUGIN_OUTPUT_CLEAR);
	tcc_add_symbol(s, "PLUGIN_OUTPUT_CLEAR", PLUGIN_OUTPUT_CLEAR);
	tcc_add_symbol(s, "plugin_summary", PLUGIN_SUMMARY);
	tcc_add_symbol(s, "PLUGIN_SUMMARY", PLUGIN_SUMMARY);
#endif

	if (__read_plugin_content(filename, buf))
		goto err_out;

	if (tcc_compile_string(s, buf) == -1)
		goto err_out;

	/* relocate the code */
	if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
		goto err_out;

	/* run init function */
	func_init func1 = (func_init)tcc_get_symbol(s, "init");
	if (func1) {
		func1();
	}

	unlock();
	return;

err_out:
	unlock();
	unload();
	::MessageBox(NULL, L"compile or load error", L"ERROR", MB_ICONERROR);
	return;
}

void CPluginTCC::dostring(const char *script_content)
{
	if (s) {
		lock();
		if (tcc_compile_string(s, script_content) == -1) {
			XTRACE(RGB(255, 0, 0), _T("CPluginTCC::dostring()@compile script error: \n%s\n"), script_content);
		} else {
			if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
				XTRACE(RGB(255, 0, 0), _T("CPluginTCC::dostring()@run script error: \n%s\n"), script_content);
		}
		unlock();
	}
}

// 这个函数内部要检测是否删除数据，如果是，则m_delete需要设置true
int CPluginTCC::handle_data(LPCTSTR protocol, void *data, int len_total, int len_data)
{
	m_function_type = PLUGIN_FUNCTION_TYPE_HANDLE_DATA;
	if (s) {
		lock();
		func_common func1 = (func_common)tcc_get_symbol(s, "handle_data");
		if (func1) {
			int ret = func1(__CWtoA(protocol), data, len_total, len_data);

			if (-1 == ret) {
				m_delete = TRUE;
			}
		}
		unlock();
	}

	m_function_type = PLUGIN_FUNCTION_TYPE_INIT;
	return 0;
}

int CPluginTCC::handle_click(LPCTSTR protocol, void *data, int len_total, int len_data)
{
	m_function_type = PLUGIN_FUNCTION_TYPE_HANDLE_CLICK;
	if (s) {
		lock();
		func_common func1 = (func_common)tcc_get_symbol(s, "handle_click");
		if (func1) {
			func1(__CWtoA(protocol), data, len_total, len_data);
		}
		unlock();
	}

	m_function_type = PLUGIN_FUNCTION_TYPE_INIT;
	return 0;
}

int CPluginTCC::handle_double(LPCTSTR protocol, void *data, int len_total, int len_data)
{
	m_function_type = PLUGIN_FUNCTION_TYPE_HANDLE_DOUBLE;
	if (s) {
		lock();
		func_common func1 = (func_common)tcc_get_symbol(s, "handle_double");
		if (func1) {
			func1(__CWtoA(protocol), data, len_total, len_data);
		}
		unlock();
	}

	m_function_type = PLUGIN_FUNCTION_TYPE_INIT;
	return 0;
}
