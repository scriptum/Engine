/*
** $Id: Mouse.c 33 2008-10-24 18:07:12Z basique $
** Library to use the mouse with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Mouse.h"

#include <string.h>

const char* buttonNames [] = {
	"l",
	"m",
	"r",
	"wu",
	"wd"
};

//static int Lua_Mouse_getX(lua_State *L) {
//	int x;
//	SDL_GetMouseState(&x, NULL);
//	lua_pushinteger(L, x);
//	return 1;
//}
//
//static int Lua_Mouse_getY(lua_State *L) {
//	int y;
//	SDL_GetMouseState(NULL, &y);
//	lua_pushinteger(L, y);
//	return 1;
//}

static int Lua_Mouse_getPos(lua_State *L) {
	int x, y;
	SDL_GetMouseState(&x, &y);
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	return 2;
}

static int Lua_Mouse_isDown(lua_State *L) {
	const char *button = luaL_checkstring(L, -1);
	Uint8 bitmask = SDL_GetMouseState(NULL, NULL);
	if (strcmp(button, buttonNames[0]) == 0)
		bitmask &= SDL_BUTTON(SDL_BUTTON_LEFT);
	else if (strcmp(button, buttonNames[1]) == 0)
		bitmask &= SDL_BUTTON(SDL_BUTTON_MIDDLE);
	else if (strcmp(button, buttonNames[2]) == 0)
		bitmask &= SDL_BUTTON(SDL_BUTTON_RIGHT);
	else
		return luaL_error(L, "Unknown option to " NAMESPACE ".mouseButtonIsDown()");
	lua_pushboolean(L, bitmask);
	return 1;
}

static const struct luaL_Reg mouselib [] = {
//	{"getMouseX", Lua_Mouse_getX},
//	{"getMouseY", Lua_Mouse_getY},
	{"getMousePos", Lua_Mouse_getPos},
	{"mouseButtonIsDown", Lua_Mouse_isDown},
	{NULL, NULL}
};

int luaopen_mouse(lua_State *L, const char *parent) {
	luaL_register(L, parent, mouselib);
	return 1;
}
