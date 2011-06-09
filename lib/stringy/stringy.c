#define LUA_LIB
#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>


static int endswith(lua_State *L) {
    const char *string = luaL_checkstring(L, 1);
    int string_len = lua_objlen(L, 1);

    const char *token = luaL_checkstring(L, 2);
    int token_len = lua_objlen(L, 2);

    int ti = token_len, si = string_len, end = 1;
    if(token_len <= string_len){
        while(ti > 0) {
            if(string[--si] != token[--ti]){ 
                end = 0;
                break;

            }
        }
    }
    else {
        end = 0;
    }
    lua_pushboolean(L, end);
    return 1;
}

static int startswith(lua_State *L) {
    const char *string = luaL_checkstring(L, 1);
    int string_len = lua_objlen(L, 1);

    const char *token = luaL_checkstring(L, 2);
    int token_len = lua_objlen(L, 2);
    int i, start = 1;
    // please make this less ugly... 
    if(token_len <= string_len){
        while(i < token_len) {
            if(string[i] != token[i]){
                start = 0;
                break;
            }
            i++;
        }
    }
    else {
        start = 0;
    }
    lua_pushboolean(L, start);
    return 1;
}

static int split(lua_State *L) {
    const char *string = luaL_checkstring(L, 1);
    const char *sep = luaL_checkstring(L, 2);
    const char *token;
    char method = 1;
    if (lua_isboolean(L, 3))
        method = !lua_toboolean(L, 3);
    int i = 1;
    lua_newtable(L);
    while ((token = strchr(string, *sep)) != NULL) {
        if(token != string || method){
          lua_pushlstring(L, string, token - string);
          lua_rawseti(L, -2, i++);
        }
        string = token + 1;
    }
    lua_pushstring(L, string);
    lua_rawseti(L, -2, i);
    return 1;
}

// http://lua-users.org/wiki/StringTrim
int strip(lua_State *L) {
    const char *front;
    const char *end;
    size_t      size;

    front = luaL_checklstring(L, 1, &size);
    end   = &front[size - 1];

    for ( ; size && isspace(*front) ; size-- , front++)
    ;
    for ( ; size && isspace(*end) ; size-- , end--)
    ;

    lua_pushlstring(L, front, (size_t)(end - front) + 1);
    return 1;
}

static const luaL_reg stringy[] = {
    {"split", split},
    {"strip", strip},
    {"startswith", startswith},
    {"endswith", endswith},
    {NULL, NULL}
};

int luaopen_stringy(lua_State *L){
    luaL_openlib(L, "stringy", stringy, 0);
    return 1;
}
