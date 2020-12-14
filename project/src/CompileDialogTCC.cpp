///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include "resource.h"
#include "../other/PluginTCC.h"
#include "../other/common.h"
#include "../src/PowerSniffApp.h"
#include "CompileDialogTCC.h"


// Definitions for the CCompileDialogTCC class
CCompileDialogTCC::CCompileDialogTCC() : CDialog(IDD_DIALOG_COMMON_COMPILE_TCC)
{
	m_hwnd_edit = NULL;
}

CCompileDialogTCC::~CCompileDialogTCC()
{
}

void CCompileDialogTCC::OnDestroy()
{
	// End the application
	//::PostQuitMessage(0);
}

INT_PTR CCompileDialogTCC::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_Resizer.HandleMessage(uMsg, wParam, lParam);

	//switch (uMsg)
	//{
	//}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL CCompileDialogTCC::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam))
    {
	case IDC_BUTTON_OK1:
		OnOK();
		return TRUE;
	case IDC_BUTTON_CANCEL1:
		OnCancel();
		return TRUE;
	case IDC_BUTTON_OPEN:
		OnOpen1();
		return TRUE;
	case IDC_BUTTON_SAVE:
		OnSave1();
		return TRUE;
	case IDC_BUTTON_SAVEAS:
		OnSaveAs1();
		return TRUE;
	case IDC_BUTTON_COMPILE:
		OnCompile1();
		return TRUE;
	case IDC_BUTTON_RUN:
		OnRun();
		return TRUE;
    }

	return FALSE;
}

BOOL CCompileDialogTCC::OnInitDialog()
{
#if 0
	CButton btn1;
	btn1.AttachDlgItem(IDC_BUTTON_RUN, *this);
	btn1.EnableWindow(FALSE);
	btn1.ShowWindow(SW_HIDE);
#endif

	SetWindowText(L"TCC Compile Test");
	m_result.AttachDlgItem(IDC_EDIT_RESULT, *this);

	// Remove taskbar item
	SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) & ~WS_EX_APPWINDOW);

	// 创建控件
	m_hwnd_edit = ::CreateWindow(L"Scintilla"
		, L"tcc edit"
		, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE
		, 6, 40, 1040, 550
		, GetHwnd()
		, NULL
		, NULL
		, NULL);
	m_fnDirect = (SciFnDirect)SendMessage(m_hwnd_edit, SCI_GETDIRECTFUNCTION, 0, 0);
	m_ptrDirect = (sptr_t)SendMessage(m_hwnd_edit, SCI_GETDIRECTPOINTER, 0, 0);

	// 初始化控件
	const char *g_szKeywords_instre1 = ""
		"asm auto bool break case catch char class const "
		"const_cast continue default delete do double "
		"dynamic_cast else enum explicit extern false finally "
		"float for friend goto if inline int long mutable "
		"namespace new operator private protected public "
		"register reinterpret_cast register return short signed "
		"sizeof static static_cast struct switch template "
		"this throw true try typedef typeid typename "
		"union unsigned using virtual void volatile "
		"wchar_t while";

	const char *g_szKeywords_instre2 = "bool char float int long short void wchar_t";

	SendEditor(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
	SendEditor(SCI_SETMARGINWIDTHN, 0, 50);
	SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 12);
	SendEditor(SCI_STYLECLEARALL);

	SendEditor(SCI_SETLEXER, SCLEX_CPP);					// TCC语法解析
	SendEditor(SCI_SETLEXERLANGUAGE, 0, (sptr_t)"cpp");
	SendEditor(SCI_SETKEYWORDS, 0, (sptr_t)g_szKeywords_instre1);		// 设置关键字
	SendEditor(SCI_SETKEYWORDS, 1, (sptr_t)g_szKeywords_instre2);		// 设置关键字

	// 下面设置各种语法元素前景色: blue, green, red
	SendEditor(SCI_STYLESETFORE, SCE_C_WORD,		0xFF0000);	// 关键字颜色
	SendEditor(SCI_STYLESETFORE, SCE_C_WORD2,		0xC08000);	// 关键字颜色

	SendEditor(SCI_STYLESETFORE, SCE_C_STRING,		0x8080FF);	// 字符串
	SendEditor(SCI_STYLESETFORE, SCE_C_CHARACTER,		0x1515A3);	// 字符
	SendEditor(SCI_STYLESETFORE, SCE_C_PREPROCESSOR,	0x808080);	// 预编译开关
	SendEditor(SCI_STYLESETFORE, SCE_C_COMMENT,		0x008000);	// 块注释
	SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTLINE,	0x008000);	// 行注释
	SendEditor(SCI_STYLESETFORE, SCE_C_NUMBER,		0x0000FF);	// 数字
	SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTDOC,	0x008000);	// 文档注释（/**开头）

	SendEditor(SCI_SETCARETLINEVISIBLE, TRUE);
	SendEditor(SCI_SETCARETLINEBACK, 0xb0ffff);
	SendEditor(SCI_SETTABWIDTH, 8);
	SendEditor(SCI_SETFOCUS, TRUE);
	SendEditor(SCI_SETCODEPAGE, SC_CP_UTF8);				// utf-8
	for (char c = 'A'; c <= 'Z'; c++) {
		if ('A' != c && 'C' != c && 'V' != c && 'Y' != c && 'Z' != c) {
			SendEditor(SCI_CLEARCMDKEY, (WPARAM)(c + (SCMOD_CTRL << 16)), SCI_NULL);
		}
	}

	// Set the Icon
	SetIconLarge(IDW_MAIN);
	SetIconSmall(IDW_MAIN);

	// 对话框缩放
	m_Resizer.Initialize(GetHwnd(), CRect(0, 0, 300, 200));
	m_Resizer.AddChild(m_result, topright, RD_STRETCH_WIDTH);
	m_Resizer.AddChild(m_hwnd_edit, bottomleft, RD_STRETCH_WIDTH | RD_STRETCH_HEIGHT);

	HICON icon = LoadIcon(GetSplitApp().m_hInstance, MAKEINTRESOURCE(IDW_MAIN));
	SetIcon(icon, TRUE);			// 设置大图标
	SetIcon(icon, FALSE);			// 设置小图标

	::SetFocus(m_hwnd_edit);
	return FALSE;
}

void CCompileDialogTCC::OnOK()
{
	CDialog::OnOK();
}

void CCompileDialogTCC::OnCancel()
{
	CDialog::OnCancel();
}

sptr_t CCompileDialogTCC::SendEditor(unsigned int iMessage, uptr_t wParam, sptr_t lParam)
{
	return m_fnDirect(m_ptrDirect, iMessage, wParam, lParam);
}

void CCompileDialogTCC::OnOpen1()
{
	LPCTSTR szFilters = L"C Files (*.c)\0*.c\0All Files (*.*)\0*.*\0";
	CFileDialog dlg(TRUE, L"c", NULL, OFN_FILEMUSTEXIST, szFilters);
	if (dlg.DoModal(*this) == IDOK) {
		CString szFilename = dlg.GetPathName();

		CFile file;
		file.Open(szFilename, CFile::modeRead);
		char *buf = (char *)malloc(SCINTILLA_MAX_BUFFER_SIZE);
		memset(buf, 0x0, SCINTILLA_MAX_BUFFER_SIZE);
		file.Read(buf, SCINTILLA_MAX_BUFFER_SIZE - 2);
		SendEditor(SCI_SETTEXT, 0, (sptr_t)buf);
		free(buf);

		m_current_filename = szFilename;
	}
}

void CCompileDialogTCC::OnSave1()
{
	if (m_current_filename.IsEmpty())
		OnSaveAs1();

	char *buf = (char *)malloc(SCINTILLA_MAX_BUFFER_SIZE);
	memset(buf, 0x0, SCINTILLA_MAX_BUFFER_SIZE);
	const long length = SendEditor(SCI_GETLENGTH);
	SendEditor(SCI_GETTEXT, length + 1, (sptr_t)buf);
	CFile file;
	file.Open(m_current_filename, CFile::modeCreate | CFile::modeWrite);
	file.Write(buf, length);
	free(buf);
}

void CCompileDialogTCC::OnSaveAs1()
{
	LPCTSTR szFilters = L"C Files (*.c)\0*.c\0All Files (*.*)\0*.*\0";
	CFileDialog dlg(FALSE, L"c", NULL, OFN_FILEMUSTEXIST, szFilters);
	if (dlg.DoModal(*this) == IDOK) {
		m_current_filename = dlg.GetPathName();
		OnSave1();
	}
}

static void __error_in_tcc(void *opaque, const char *str)
{
	CCompileDialogTCC *pDialog = (CCompileDialogTCC*)opaque;
	if (pDialog)
		pDialog->m_result.SetWindowText(__CAtoW(str));
}

static int __libtcc_test_compile(CCompileDialogTCC *pDialog, const char *buffer)
{
	TCCState *s = NULL;

	s = tcc_new();
	if (!s) {
		pDialog->m_result.SetWindowText(L"can't init tcc virtual machine\n");
		return -1;
	}
	tcc_set_error_func(s, pDialog, ::__error_in_tcc);

#if 1
	/* if tcclib.h and libtcc1.a are not installed, where can we find them */
	TCHAR filepath[256] = L"";
	TCHAR temp1[256] = L"";
	GetModuleFileName(NULL, filepath, MAX_PATH);
	TCHAR *p = wcsrchr(filepath, _T('\\')); p++; *p = _T('\0');
	SetCurrentDirectory(filepath);
#endif

#if 1
	wcscpy(temp1, filepath);
	wcscat(temp1, L"3rd\\libtcc\\");
	tcc_set_lib_path(s, __CWtoA(temp1));		// 文件夹里面必须有libtcc使用的lib和include
#endif

#if 1
	wcscpy(temp1, filepath);
	tcc_add_sysinclude_path(s, __CWtoA(temp1));
	tcc_add_include_path(s, __CWtoA(temp1));
	tcc_add_library_path(s, __CWtoA(temp1));
	tcc_add_library(s, __CWtoA(temp1));

	wcscat(temp1, L"lib\\");
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

	if (tcc_compile_string(s, buffer) == -1) {
		// compile error
	} else {
		pDialog->m_result.SetWindowText(_T("compile success\n"));
	}

	/* delete the state */
	tcc_delete(s);
	return 0;
}

void CCompileDialogTCC::OnCompile1()
{
	int len_ = SendEditor(SCI_GETLENGTH) + 1;
	char *buffer = (char *)malloc(len_);
	memset(buffer, 0x0, len_);
	if (buffer) {
		SendEditor(SCI_GETTEXT, len_, (LPARAM)buffer);
		__libtcc_test_compile(this, buffer);			// 编译测试
		free(buffer);
	}
}

// TCC暂时没有run逻辑
void CCompileDialogTCC::OnRun()
{
#if 1
	MessageBox(L"NOT support, wait for new version.", L"ERROR", MB_OK);
#else
	CPluginBase *m_plugin = GetSplitApp().GetMainFrame()->m_pPlugin;
	if (m_plugin) {
		std::string path = __CWtoA(m_plugin->getPluginPath());
		std::transform(path.begin(), path.end(), path.begin(), [](char c) {return static_cast<char>(::tolower(c)); });

		std::string tail = ".c";
		bool endwith = path.compare(path.size() - tail.size(), tail.size(), tail) == 0;

		if (endwith) {
			/*
			int len_ = SendEditor(SCI_GETLENGTH) + 1;
			char *buffer = (char *)malloc(len_);
			if (buffer) {
				int len = SendEditor(SCI_GETTEXT, len_, (LPARAM)buffer);
				SendMessage(GetParent(), WM_USER_RUN_TCC, (WPARAM)buffer, (LPARAM)len);
				free(buffer);
			}
			*/
			MessageBox(L"C language NOT support, wait for new version.", L"ERROR", MB_OK);
		} else {
			MessageBox(L"Current plugin is not c script.", L"ERROR", MB_OK);
		}
	} else {
		MessageBox(L"Need select a plugin on menu.", L"ERROR", MB_OK);
	}
#endif
}
