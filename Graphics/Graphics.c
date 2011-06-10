/*
** $Id: Graphics.c 56 2009-09-03 21:43:48Z basique $
** Graphic-related functions and library to use them with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>



#include "../SOIL/SOIL.h"
#include "../Main.h"
#include "../Macros.h"
#include "Graphics.h"
#include "../physfsrwops.h"

int Lua_Graphics_showCursor(lua_State *L) {
	int result;
	if (lua_isnoneornil(L, 1)) {
		result = SDL_ShowCursor(SDL_QUERY);
		if (result == SDL_ENABLE)
			lua_pushboolean(L, 1);
		else
			lua_pushboolean(L, 0);
	} else {
		luaL_checkany(L, -1);
		result = lua_toboolean(L, 1);
		lua_pushnil(L);
		if (result)
			SDL_ShowCursor(SDL_ENABLE);
		else
			SDL_ShowCursor(SDL_DISABLE);
	}
	return 1;
}

int Lua_Graphics_getTicks(lua_State *L) {
	lua_pushinteger(L, SDL_GetTicks());
	return 1;
}

int Lua_Graphics_translateView(lua_State *L) {
	GLdouble translateX = (GLdouble)luaL_checknumber(L, 1);
	GLdouble translateY = (GLdouble)luaL_checknumber(L, 2);
	glTranslated(translateX, translateY, 0);
	return 0;
}

int Lua_Graphics_scaleView(lua_State *L) {
	GLdouble scaleX = (GLdouble)luaL_checknumber(L, 1);
	GLdouble scaleY;
	if(lua_isnumber(L, 2))
        scaleY = (GLdouble)luaL_checknumber(L, 2);
    else
        scaleY = scaleX;
	glScaled(scaleX, scaleY, 0);
	return 0;
}

int Lua_Graphics_rotateView(lua_State *L) {
	GLdouble rotate = (GLdouble)luaL_checknumber(L, 1);
	glRotated(rotate, 0, 0, 1);
	return 0;
}

int Lua_Graphics_saveView(lua_State *L) {
	glPushMatrix();
	if (glGetError() == GL_STACK_OVERFLOW)
		return luaL_error(L, "No more free slots to save the view.");
	return 0;
}

int Lua_Graphics_restoreView(lua_State *L) {
	glPopMatrix();
	if (glGetError() == GL_STACK_UNDERFLOW)
		return luaL_error(L, "No saved view was found.");
	return 0;
}

int Lua_Graphics_resetView(lua_State *L) {
	UNUSED(L);
	glLoadIdentity();
	return 0;
}



static const struct luaL_Reg graphicslib [] = {
	{"init",				Lua_Graphics_init},
	{"getWidth",		Lua_Graphics_getWindowWidth},
	{"getHeight",		Lua_Graphics_getWindowHeight},
	{"getSize",		Lua_Graphics_getWindowSize},
	{"showCursor",			Lua_Graphics_showCursor},
	{"getTicks",			Lua_Graphics_getTicks},
	{"translate",		Lua_Graphics_translateView},
	{"scale",		    	Lua_Graphics_scaleView},
	{"rotate",			Lua_Graphics_rotateView},
	{"push",			Lua_Graphics_saveView},
	{"pop",			Lua_Graphics_restoreView},
	{"reset",			Lua_Graphics_resetView},
	{"setColor",			Lua_Graphics_setColor},
	{"setBlendMode",		Lua_Graphics_setBlendMode},
	{"newImage",			Lua_Image_load},
	{"imageFromString",	Lua_Image_loadFromString},
	{"line",				Lua_Graphics_line},
	{"rectangle",			Lua_Graphics_rectangle},
	{"point",		    	Lua_Graphics_point},
	{"circle",			    Lua_Graphics_circle},
	{"setLineWidth",		Lua_Graphics_setLineWidth},
    {"setPointSize",		Lua_Graphics_setPointSize},
	{"setSmooth",		    Lua_Graphics_setSmooth},


	{NULL, NULL}
};

static const struct luaL_Reg imagelib_m [] = {
	{"__gc",				image_gc},
	{"__tostring",			image_tostring},
	{"getWidth",			Lua_Image_getWidth},
	{"getHeight",			Lua_Image_getHeight},
	{"getTextureId",		Lua_Image_getTextureId},
	{"getSize",				Lua_Image_getSize},
	{"draw",				Lua_Image_draw},
	{"drawq",			    Lua_Image_drawq},
	{NULL, NULL}
};

int luaopen_graphics(lua_State *L, const char *parent) {
	luaL_newmetatable(L, "scrupp.image");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, imagelib_m);
	lua_pop(L, 1);	/* pop the metatable */
	luaL_register(L, parent, graphicslib);
	return 1;
}
