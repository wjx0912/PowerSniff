#pragma once

#include "PluginBase.h"
#include "libtcc.h"

void CheckLibTcc();

// libtcc.dllµ¼³öº¯Êý
extern tcc_new1				tcc_new;
extern tcc_delete1			tcc_delete;
extern tcc_set_lib_path1		tcc_set_lib_path;
extern tcc_set_error_func1		tcc_set_error_func;
extern tcc_set_options1			tcc_set_options;
extern tcc_add_include_path1		tcc_add_include_path;
extern tcc_add_sysinclude_path1		tcc_add_sysinclude_path;
extern tcc_define_symbol1		tcc_define_symbol;
extern tcc_undefine_symbol1		tcc_undefine_symbol;
extern tcc_add_file1			tcc_add_file;
extern tcc_compile_string1		tcc_compile_string;
extern tcc_set_output_type1		tcc_set_output_type;
extern tcc_add_library_path1		tcc_add_library_path;
extern tcc_add_library1			tcc_add_library;
extern tcc_add_symbol1			tcc_add_symbol;
extern tcc_output_file1			tcc_output_file;
extern tcc_run1				tcc_run;
extern tcc_relocate1			tcc_relocate;
extern tcc_get_symbol1			tcc_get_symbol;

//#define __cdecl			__attribute__((__cdecl__))
//#define __stdcall			__attribute__((__stdcall__))
int libtcc_test();

class CPluginTCC : public CPluginBase
{
public:
	CPluginTCC()
	{
		s = NULL;
	}

	void reload(LPCTSTR filename);
	void unload();
	void dostring(const char *script_content);
	int handle_data(LPCTSTR protocol, void *data, int len_total, int len_data);
	int handle_click(LPCTSTR protocol, void *data, int len_total, int len_data);
	int handle_double(LPCTSTR protocol, void *data, int len_total, int len_data);

private:
	TCCState		*s;
};
