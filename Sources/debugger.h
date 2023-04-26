#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MINILUA
#include "minilua.h"
#elif LUAU
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#else
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#endif


void start_debugger(lua_State* lua_state,int port, void (*on_ready)(void));
void waitfor_debugger(void);

#ifdef __cplusplus
}
#endif