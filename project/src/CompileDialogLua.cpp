///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include <algorithm>
#include "resource.h"
#include "../other/LuaSource.h"
#include "../other/common.h"
#include "../src/PowerSniffApp.h"
#include "CompileDialogLua.h"


// Definitions for the CCompileDialogLua class
CCompileDialogLua::CCompileDialogLua() : CDialog(IDD_DIALOG_COMMON_COMPILE_LUA)
{
	m_hwnd_edit = NULL;
}

CCompileDialogLua::~CCompileDialogLua()
{
}

void CCompileDialogLua::OnDestroy()
{
	// End the application
	//::PostQuitMessage(0);
}

INT_PTR CCompileDialogLua::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_Resizer.HandleMessage(uMsg, wParam, lParam);

	//switch (uMsg)
	//{
	//}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL CCompileDialogLua::OnCommand(WPARAM wParam, LPARAM lParam)
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

BOOL CCompileDialogLua::OnInitDialog()
{
#if 0
	CButton btn1;
	btn1.AttachDlgItem(IDC_BUTTON_RUN, *this);
	btn1.EnableWindow(FALSE);
	btn1.ShowWindow(SW_HIDE);
#endif

	SetWindowText(L"Lua Compile Test");
	m_result.AttachDlgItem(IDC_EDIT_RESULT, *this);

	// Remove taskbar item
	SetWindowLong(GWL_EXSTYLE, GetWindowLong(GWL_EXSTYLE) & ~WS_EX_APPWINDOW);

	// 创建控件
	m_hwnd_edit = ::CreateWindow(L"Scintilla"
		, L"lua edit"
		, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE
		, 6, 40, 1040, 550
		, GetHwnd()
		, NULL
		, NULL
		, NULL);
	m_fnDirect = (SciFnDirect)SendMessage(m_hwnd_edit, SCI_GETDIRECTFUNCTION, 0, 0);
	m_ptrDirect = (sptr_t)SendMessage(m_hwnd_edit, SCI_GETDIRECTPOINTER, 0, 0);

	// 初始化控件
	const char *g_szKeywords_instre1 = "and break do else elseif end false for function goto if in local nil not or repeat return then true until while ";
	const char *g_szKeywords_instre2 = "_ENV _G _VERSION assert collectgarbage dofile error getfenv getmetatable ipairs load loadfile loadstring module "\
					"next pairs pcall print rawequal rawget rawlen rawset require select setfenv setmetatable tonumber tostring type "\
					"unpack xpcall string table math bit32 coroutine io os debug package __index __newindex __call __add __sub __mul "\
					"__div __mod __pow __unm __concat __len __eq __lt __le __gc __mode ";
	const char *g_szKeywords_type1 = "byte char dump find format gmatch gsub len lower match rep reverse sub upper abs acos asin atan atan2 ceil cos "\
					"cosh deg exp floor fmod frexp ldexp log log10 max min modf pow rad random randomseed sin sinh sqrt tan tanh arshift "\
					"band bnot bor btest bxor extract lrotate lshift replace rrotate rshift shift string.byte string.char string.dump "\
					"string.find string.format string.gmatch string.gsub string.len string.lower string.match string.rep string.reverse "\
					"string.sub string.upper table.concat table.insert table.maxn table.pack table.remove table.sort table.unpack math.abs "\
					"math.acos math.asin math.atan math.atan2 math.ceil math.cos math.cosh math.deg math.exp math.floor math.fmod math.frexp "\
					"math.huge math.ldexp math.log math.log10 math.max math.min math.modf math.pi math.pow math.rad math.random math.randomseed "\
					"math.sin math.sinh math.sqrt math.tan math.tanh bit32.arshift bit32.band bit32.bnot bit32.bor bit32.btest bit32.bxor "\
					"bit32.extract bit32.lrotate bit32.lshift bit32.replace bit32.rrotate bit32.rshift ";
	const char *g_szKeywords_type2 = "close flush lines read seek setvbuf write clock date difftime execute exit getenv remove rename setlocale time tmpname "\
					"coroutine.create coroutine.resume coroutine.running coroutine.status coroutine.wrap coroutine.yield io.close io.flush "\
					"io.input io.lines io.open io.output io.popen io.read io.tmpfile io.type io.write io.stderr io.stdin io.stdout os.clock "\
					"os.date os.difftime os.execute os.exit os.getenv os.remove os.rename os.setlocale os.time os.tmpname debug.debug debug.getfenv "\
					"debug.gethook debug.getinfo debug.getlocal debug.getmetatable debug.getregistry debug.getupvalue debug.getuservalue debug.setfenv "\
					"debug.sethook debug.setlocal debug.setmetatable debug.setupvalue debug.setuservalue debug.traceback debug.upvalueid "\
					"debug.upvaluejoin package.cpath package.loaded package.loaders package.loadlib package.path package.preload package.seeall ";

	SendEditor(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
	SendEditor(SCI_SETMARGINWIDTHN, 0, 50);
	SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 12);
	SendEditor(SCI_STYLECLEARALL);

	SendEditor(SCI_SETLEXER, SCLEX_LUA);					// Lua语法解析
	SendEditor(SCI_SETKEYWORDS, 0, (sptr_t)g_szKeywords_instre1);		// 设置关键字
	SendEditor(SCI_SETKEYWORDS, 1, (sptr_t)g_szKeywords_instre2);		// 设置关键字
	SendEditor(SCI_SETKEYWORDS, 2, (sptr_t)g_szKeywords_type1);		// 设置关键字
	SendEditor(SCI_SETKEYWORDS, 3, (sptr_t)g_szKeywords_type2);		// 设置关键字

	// 下面设置各种语法元素前景色: blue, green, red
	SendEditor(SCI_STYLESETFORE, SCE_LUA_WORD,		0xFF0000);	// 关键字颜色
	SendEditor(SCI_STYLESETFORE, SCE_LUA_WORD2,		0xC08000);	// 关键字颜色
	SendEditor(SCI_STYLESETFORE, SCE_LUA_WORD3,		0xFF0080);	// 关键字颜色
	SendEditor(SCI_STYLESETFORE, SCE_LUA_WORD4,		0xA00000);	// 关键字颜色
	SendEditor(SCI_STYLESETITALIC, SCE_LUA_WORD4, TRUE);			// 斜体

	SendEditor(SCI_STYLESETFORE, SCE_LUA_STRING,		0x8080FF);	// 字符串
	SendEditor(SCI_STYLESETFORE, SCE_LUA_CHARACTER,		0x1515A3);	// 字符
	SendEditor(SCI_STYLESETFORE, SCE_LUA_PREPROCESSOR,	0x808080);	// 预编译开关
	SendEditor(SCI_STYLESETFORE, SCE_LUA_COMMENT,		0x008000);	// 块注释
	SendEditor(SCI_STYLESETFORE, SCE_LUA_COMMENTLINE,	0x008000);	// 行注释
	SendEditor(SCI_STYLESETFORE, SCE_LUA_NUMBER,		0x0000FF);	// 数字
	SendEditor(SCI_STYLESETFORE, SCE_LUA_COMMENTDOC,	0x008000);	// 文档注释（/**开头）

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

void CCompileDialogLua::OnOK()
{
	CDialog::OnOK();
}

void CCompileDialogLua::OnCancel()
{
	CDialog::OnCancel();
}

sptr_t CCompileDialogLua::SendEditor(unsigned int iMessage, uptr_t wParam, sptr_t lParam)
{
	return m_fnDirect(m_ptrDirect, iMessage, wParam, lParam);
}

void CCompileDialogLua::OnOpen1()
{
	LPCTSTR szFilters = L"Lua Files (*.lua)\0*.lua\0All Files (*.*)\0*.*\0";
	CFileDialog dlg(TRUE, L"lua", NULL, OFN_FILEMUSTEXIST, szFilters);
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

void CCompileDialogLua::OnSave1()
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

void CCompileDialogLua::OnSaveAs1()
{
	LPCTSTR szFilters = L"Lua Files (*.lua)\0*.lua\0All Files (*.*)\0*.*\0";
	CFileDialog dlg(FALSE, L"lua", NULL, OFN_FILEMUSTEXIST, szFilters);
	if (dlg.DoModal(*this) == IDOK) {
		m_current_filename = dlg.GetPathName();
		OnSave1();
	}
}

void CCompileDialogLua::OnCompile1()
{
	int len_ = SendEditor(SCI_GETLENGTH) + 1;
	char *buffer = (char *)malloc(len_);
	memset(buffer, 0x0, len_);
	if (buffer) {
		SendEditor(SCI_GETTEXT, len_, (LPARAM)buffer);
		{
			lua_State *L;
			L = luaL_newstate();
			luaL_openlibs(L);
			int ret = luaL_loadstring(L, buffer);
			if (ret)
			{
				const char *error_info = lua_tostring(L, -1);
				TCHAR error_info_w[512] = L"";
				::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)error_info, -1, error_info_w, 512);
				TCHAR *p = wcsstr(error_info_w, L"]:");
				if (p)
				{
					p++;
					p++;
					TCHAR show_buf[512] = L"error line ";
					wcscat(show_buf, p);
					m_result.SetWindowText(show_buf);
				}
			}
			else
			{
				m_result.SetWindowText(L"compile success");
			}
			lua_close(L);
		}
		free(buffer);
	}
}

void CCompileDialogLua::OnRun()
{
#if 1
	MessageBox(L"NOT support, wait for new version.", L"ERROR", MB_OK);
#else
	CPluginBase *m_plugin = GetSplitApp().GetMainFrame()->m_pPlugin;
	if (m_plugin) {
		std::string path = __CWtoA(m_plugin->getPluginPath());
		std::transform(path.begin(), path.end(), path.begin(), [](char c) {return static_cast<char>(::tolower(c)); });

		std::string tail = ".lua";
		bool endwith = path.compare(path.size() - tail.size(), tail.size(), tail) == 0;

		if (endwith) {
			int len_ = SendEditor(SCI_GETLENGTH) + 1;
			char *buffer = (char *)malloc(len_);
			if (buffer) {
				int len = SendEditor(SCI_GETTEXT, len_, (LPARAM)buffer);
				SendMessage(GetParent(), WM_USER_RUN_LUA, (WPARAM)buffer, (LPARAM)len);
				free(buffer);
			}
		} else {
			MessageBox(L"Current plugin is not lua script.", L"ERROR", MB_OK);
		}
	} else {
		MessageBox(L"Need select a plugin on menu.", L"ERROR", MB_OK);
	}
#endif
}
