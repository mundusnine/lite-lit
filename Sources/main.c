#include <stdio.h>
#include "fenster.h"
#include "renderer.h"
#include "debugger.h"
#include "api/api.h"

// "Windows"
// "Mac OS X"
// "Linux"
// "iOS"
// "Android"
#ifdef _WIN32
  #include <windows.h>
  #define PLATFORM "Windows"
#elif __linux__
  #include <unistd.h>
  #define PLATFORM "Linux"
#elif __APPLE__
  #include <mach-o/dyld.h>
  #define PLATFORM "Mac OS X"
#endif


// SDL_Window *window;


static double get_scale(void) {
  float dpi;
#if _WIN32
  dpi = (float)GetDpiForSystem();
  return dpi / 96.0;
#else
  return 1.0;
#endif
}


static void get_exe_filename(char *buf, int sz) {
#if _WIN32
  int len = GetModuleFileNameA(NULL, buf, sz - 1);
  buf[len] = '\0';
#elif __linux__
  char path[512];
  sprintf(path, "/proc/%d/exe", getpid());
  int len = readlink(path, buf, sz - 1);
  buf[len] = '\0';
#elif __APPLE__
  unsigned size = sz;
  _NSGetExecutablePath(buf, &size);
#else
  strcpy(buf, "./lite");
#endif
}


static void init_window_icon(void) {
#ifndef _WIN32
  #include "../icon.inl"
  (void) icon_rgba_len; /* unused */
  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
    icon_rgba, 64, 64,
    32, 64 * 4,
    0x000000ff,
    0x0000ff00,
    0x00ff0000,
    0xff000000);
  SDL_SetWindowIcon(window, surf);
  SDL_FreeSurface(surf);
#endif
}
static lua_State *L = NULL;
void ready(void) {

  (void) luaL_dostring(L,
  "local core\n"
  "  print(\"Yolo\")"
  "xpcall(function()\n"
  "  SCALE = tonumber(os.getenv(\"LITE_SCALE\")) or SCALE\n"
  "  PATHSEP = package.config:sub(1, 1)\n"
  "  EXEDIR = EXEFILE:match(\"^(.+)[/\\\\].*$\")\n"
  "  package.path = EXEDIR .. '/data/?.lua;' .. package.path\n"
  "  package.path = EXEDIR .. '/data/?/init.lua;' .. package.path\n"
  "  core = require('core')\n"
  "  core.init()\n"
  "  core.run()\n"
  "end, function(err)\n"
  "  print('Error: ' .. tostring(err))\n"
  "  print(debug.traceback(nil, 2))\n"
  "  if core and core.on_error then\n"
  "    pcall(core.on_error, err)\n"
  "  end\n"
  "  os.exit(1)\n"
  "end)");
}

fenster_t* f;
int main(int argc, char **argv) {
    int width = 1280;
    int height = 720;
    #ifdef _WIN32
    SetProcessDPIAware();

    RECT rect;
    BOOL success = SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
    if (success) {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
    #endif

    f = (fenster_t*)malloc(sizeof(fenster_t));
    memset(f, 0, sizeof(fenster_t));
    f->title = "lit";
    f->width = width * 0.8;
    f->height = height * 0.8;
    f->buf = (uint32_t*)malloc(sizeof(uint32_t) * f->width * f->height);
    memset(f->buf, 0, sizeof(uint32_t) * f->width * f->height);
    fenster_open(f);
    // SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    // SDL_EnableScreenSaver();
    // SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    // atexit(SDL_Quit);

    //#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR /* Available since 2.0.8 */
    //  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    //#endif
    //#if SDL_VERSION_ATLEAST(2, 0, 5)
    //  SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    //#endif

    // SDL_DisplayMode dm;
    // SDL_GetCurrentDisplayMode(0, &dm);

    // window = SDL_CreateWindow(
    //   "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dm.w * 0.8, dm.h * 0.8,
    //   SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);
    init_window_icon();
    ren_init(f);


    L = luaL_newstate();
    luaL_openlibs(L);
    api_load_libs(L);


    lua_newtable(L);
    for (int i = 0; i < argc; i++) {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, "ARGS");

    lua_pushstring(L, "1.0");
    lua_setglobal(L, "VERSION");

    lua_pushstring(L, PLATFORM);
    lua_setglobal(L, "PLATFORM");

    lua_pushnumber(L, get_scale());
    lua_setglobal(L, "SCALE");

    char exename[2048];
    get_exe_filename(exename, sizeof(exename));
    lua_pushstring(L, exename);
    lua_setglobal(L, "EXEFILE");

    ready();
    // start_debugger(L, -1,ready);

    // waitfor_debugger();

    lua_close(L);
    fenster_close(f);
    // SDL_DestroyWindow(window);

    return EXIT_SUCCESS;
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
    int nCmdShow) {

    LPWSTR* argvW;
    char** argvA;
    int argc;

    LPWSTR cmdLine = GetCommandLineW();

    argvW = CommandLineToArgvW(cmdLine, &argc);

    argvA = (char**)malloc(argc * sizeof(char*));

    for (int i = 0; i < argc; i++)
    {
        int len = WideCharToMultiByte(CP_ACP, 0, argvW[i], -1, NULL, 0, NULL, NULL);

        argvA[i] = (char*)malloc(len * sizeof(char));

        WideCharToMultiByte(CP_ACP, 0, argvW[i], -1, argvA[i], len, NULL, NULL);
    }

    
    int ret = main(argc, argvA);
    for (int i = 0; i < argc; i++)
    {
        free(argvA[i]);
    }
    free(argvA);

    LocalFree(argvW);
    return ret;
}
#endif
