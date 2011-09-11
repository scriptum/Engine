#ifndef __THREAD_H__
#define __THREAD_H__

#include "lua.h"


typedef struct Lua_Thread {
    lua_State *L;
    const char * file;
    SDL_Thread * thread;
} Lua_Thread;

int luaopen_thread(lua_State *L, const char *parent);
void thread_init();
#endif /* __THREAD_H__ */
