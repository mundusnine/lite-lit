#include "debugger.h"

#include "lrdb/server.hpp"

using namespace lrdb;
static server* lua_debug_server = nullptr;
static void (*debug_on_ready)(void) = NULL;
static bool did_ready = false;
void start_debugger(lua_State* lua_state, int port, void (*on_ready)(void))
{
	if (port <= 0) {
		port = 21111;
	}

	if (lua_debug_server == nullptr) {
		lua_debug_server = new server(port);
	}
	debug_on_ready = on_ready;
	lua_debug_server->reset(lua_state);
	lua_debug_server->command_stream().wait_for_connection();
}

void waitfor_debugger(void)
{
	
	// If no breakpoints are specified beforehand run_one will block
	// This is intended behavior,we want users to be able to attach 
	// and then specify what line they want to break at.
	// If a breakpoint already exists it will enable us to actually wait and then call ready
	// Not doing this way would make it so that the lua code would run before the breakpoint being set.
	lua_debug_server->command_stream().run_one();
	if (!did_ready) {
		did_ready = true;
		debug_on_ready();
	}
}