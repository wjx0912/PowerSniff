#ifndef LUA_WRAPPER_20150705_H
#define LUA_WRAPPER_20150705_H
#pragma warning (disable : 4819)
#pragma warning (disable : 4091)
#include "PluginBase.h"
#include "LuaSource.h"
#include "wxx_file.h"

class CPluginLua : public CPluginBase
{
public:
	CPluginLua()
	{
		L = NULL;
	}
	void reload(LPCTSTR filename);
	void unload();
	void dostring(const char *script_content);
	int handle_data(LPCTSTR protocol, void *data, int len_total, int len_data);
	int handle_click(LPCTSTR protocol, void *data, int len_total, int len_data);
	int handle_double(LPCTSTR protocol, void *data, int len_total, int len_data);

private:
	lua_State		*L;
};

#endif
