#include "stdafx.h"
#include "PluginLua.h"
#include "../src/PowerSniffApp.h"
#include "../src/Mainfrm.h"

int LUA_XTRACE(lua_State* L)
{
	const char *out_str = luaL_checkstring(L, 1);
	if (NULL == out_str)
		return -1;

	XTRACE(CAtoW(out_str, CP_UTF8));
	return 0;
}

int LUA_XTRACE_RAW(lua_State* L)
{
	const char *out_str = luaL_checkstring(L, 1);
	if (NULL == out_str)
		return -1;

	XTRACE(CAtoW(out_str, CP_UTF8));
	return 0;
}

int LUA_DBGVIEW(lua_State* L)
{
	const char *out_str = luaL_checkstring(L, 1);
	if (NULL == out_str)
		return -1;

	::OutputDebugString(CAtoW(out_str, CP_UTF8));
	return 0;
}

// 参数：字符串，字体颜色，字体大小
int LUA_PLUGIN_OUTPUT(lua_State* L)
{
	const char *textcontentA = luaL_checkstring(L, 1);
	if (NULL == textcontentA)
		return -1;
	Win32xx::CString textcontent = CAtoW(textcontentA, CP_UTF8);

	GetSplitApp().GetMainFrame()->m_pDockTextBottomRightPluginOutput->TextAppend(RGB(0x00, 0x0, 0)
		, DEFAULT_RICHEDIT_TEXT_SIZE, (TCHAR *)textcontent.c_str());
	return 0;
}

int PLUGIN_OUTPUT_CLEAR(lua_State* L)
{
	GetSplitApp().GetMainFrame()->m_pDockTextBottomRightPluginOutput->TextClear();
	return 0;
}

int PLUGIN_SUMMARY(lua_State* L)
{
	const char *textcontentA = luaL_checkstring(L, 1);
	if (NULL == textcontentA)
		return -1;
	Win32xx::CString textcontent = CAtoW(textcontentA, CP_UTF8);

	GetSplitApp().GetMainFrame()->SetCurrentItemSummary((TCHAR *)textcontent.c_str());
	return 0;
}

void CPluginLua::unload()
{
	lock();
	if (L) {
		lua_close(L);
		L = NULL;
	}
	unlock();
}

void CPluginLua::reload(LPCTSTR filename)
{
	wcscpy(m_plugin_filename, filename);
	unload();

	lock();
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dostring(L, "package.path = package.path .. ';./plugin/?.lua;./3rd/?.lua;./lib/?.lua'");
	lua_register(L, "trace", LUA_XTRACE);
	lua_register(L, "TRACE", LUA_XTRACE);
	lua_register(L, "xtrace", LUA_XTRACE);
	lua_register(L, "XTRACE", LUA_XTRACE);
	lua_register(L, "trace_raw", LUA_XTRACE_RAW);
	lua_register(L, "TRACE_RAW", LUA_XTRACE_RAW);
	lua_register(L, "xtrace_raw", LUA_XTRACE_RAW);
	lua_register(L, "XTRACE_RAW", LUA_XTRACE_RAW);
	lua_register(L, "dbgview", LUA_DBGVIEW);
	lua_register(L, "DBGView", LUA_DBGVIEW);
	lua_register(L, "plugin_output", LUA_PLUGIN_OUTPUT);
	lua_register(L, "PLUGIN_OUTPUT", LUA_PLUGIN_OUTPUT);
	lua_register(L, "plugin_output_clear", PLUGIN_OUTPUT_CLEAR);
	lua_register(L, "PLUGIN_OUTPUT_CLEAR", PLUGIN_OUTPUT_CLEAR);
	lua_register(L, "plugin_summary", PLUGIN_SUMMARY);
	lua_register(L, "PLUGIN_SUMMARY", PLUGIN_SUMMARY);

	// 获取插件路径
	TCHAR findpath[256] = _T("");
	GetModuleFileName(NULL, findpath, MAX_PATH);
	TCHAR *p = wcsrchr(findpath, _T('\\'));
	*p = _T('\0');
	wcscat(findpath, _T("\\plugin\\"));
	wcscat(findpath, filename);
	luaL_dofile(L, __CWtoA(findpath));

	// 执行初始化函数
	int ret = lua_getglobal(L, "init");		// 把全局变量 name 里的值压栈，返回该值的类型，跟入lua源代码，即便没有init函数，栈中也会加1
	if (ret > 0) {
		lua_pcall(L, 0, 0, 0);
	}
	else {
		lua_pop(L, 1);
	}
	unlock();
}

void CPluginLua::dostring(const char *script_content)
{
	if (L) {
		lock();
		luaL_dostring(L, script_content);
		unlock();
	}
}

int CPluginLua::handle_data(LPCTSTR protocol, void *data, int len_total, int len_data)
{
	m_function_type = PLUGIN_FUNCTION_TYPE_HANDLE_DATA;
	if (L) {
		lock();
		int ret = lua_getglobal(L, "handle_data");
		if (ret > 0) {
			lua_pushstring(L, __CWtoA(protocol));
			lua_pushlstring(L, (const char *)data, len_total);
			lua_pushinteger(L, len_total);
			lua_pushinteger(L, len_data);
			lua_pcall(L, 4, 1, 0);

			// 判断是否要删除包
			const char *ret_string = lua_tostring(L, -1);
			if (NULL != ret_string) {
				if (0 == strcmp(ret_string, "delete")) {
					m_delete = TRUE;
				}
			}
		}
		lua_pop(L, 1);
		unlock();
	}

	m_function_type = PLUGIN_FUNCTION_TYPE_INIT;
	return 0;
}

int CPluginLua::handle_click(LPCTSTR protocol, void *data, int len_total, int len_data)
{
	m_function_type = PLUGIN_FUNCTION_TYPE_HANDLE_CLICK;
	if (L) {
		lock();
		int ret = lua_getglobal(L, "handle_click");
		if (ret > 0) {
			lua_pushstring(L, __CWtoA(protocol));
			lua_pushlstring(L, (const char *)data, len_total);
			lua_pushinteger(L, len_total);
			lua_pushinteger(L, len_data);
			lua_pcall(L, 4, 1, 0);
		}
		lua_pop(L, 1);
		unlock();
	}

	m_function_type = PLUGIN_FUNCTION_TYPE_INIT;
	return 0;
}

int CPluginLua::handle_double(LPCTSTR protocol, void *data, int len_total, int len_data)
{
	m_function_type = PLUGIN_FUNCTION_TYPE_HANDLE_DOUBLE;
	if (L) {
		lock();
		int ret = lua_getglobal(L, "handle_double");
		if (ret > 0) {
			lua_pushstring(L, __CWtoA(protocol));
			lua_pushlstring(L, (const char *)data, len_total);
			lua_pushinteger(L, len_total);
			lua_pushinteger(L, len_data);
			lua_pcall(L, 4, 1, 0);
		}
		lua_pop(L, 1);
		unlock();
	}

	m_function_type = PLUGIN_FUNCTION_TYPE_INIT;
	return 0;
}
