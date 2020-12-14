#ifndef LUA_SOURCE_H
#define LUA_SOURCE_H

// 使用哪个版本LUA
//#define USE_LUA_5_1_5
#define USE_LUA_5_3_5

// ===================================================
// 从下面开始就不要改了
#ifdef USE_LUA_5_1_5
#pragma warning (disable : 4013)
#pragma warning (disable : 4310)
#pragma warning (disable : 4244)
#pragma warning (disable : 4702)
#pragma warning (disable : 4709)
#pragma warning (disable : 4996)
#pragma warning (disable : 4047)
#pragma warning (disable : 4133)
#define LUA_CORE
#define loslib_c
#define LUA_LIB
extern "C" {
#include "3rd/lua/src/lua.h"
#include "3rd/lua/src/lualib.h"
#include "3rd/lua/src/lauxlib.h"
}
#endif

#ifdef USE_LUA_5_3_5
#pragma warning (disable : 4310)
#pragma warning (disable : 4244)
#pragma warning (disable : 4702)
#pragma warning (disable : 4709)
#pragma warning (disable : 4996)
#pragma warning (disable : 4047)
#pragma warning (disable : 4133)
#include "3rd/lua/src/lua.hpp"
#endif

#endif
