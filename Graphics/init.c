#include <math.h>

#include "../SOIL/SOIL.h"
#include "../Main.h"
#include "../Macros.h"
#include "Graphics.h"
#include "../physfsrwops.h"
#include "../render.h"

Uint32 flags;

void resize(int width, int height)
{
	screen = SDL_SetVideoMode(width, height, 32, flags);
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, width, height, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

int Lua_Graphics_init(lua_State *L) {
	char * appName = (char *)luaL_checkstring(L, 1);
	int width = luaL_checkint(L, 2);
	int height = luaL_checkint(L, 3);
	char bpp = luaL_checkint(L, 4);
	char fullscreen = lua_toboolean(L, 5);
	char resizable = lua_toboolean(L, 6);
	char vsync = lua_toboolean(L, 7);

	flags = SDL_OPENGL;

	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	} else if (resizable) {
		flags |= SDL_RESIZABLE;
	}
	if (screen == NULL) {
		if ( SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) != 0 )
			return luaL_error(L, "Couldn't initialize SDL: %s", SDL_GetError ());
		atexit(SDL_Quit);
	}
	/* set window caption */
	SDL_WM_SetCaption (appName, appName);
	/* enable double buffering */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
//	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, vsync);
	screen = SDL_SetVideoMode(width, height, 32, flags);
	if (screen == NULL)
		return luaL_error(L, 	"Couldn't set %dx%dx%d video mode: %s",
								width, height, bpp, SDL_GetError());
	initRenderer();
	/* set background color */
	glClearColor( 0, 0, 0, 1);
	/* set line antialiasing */
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	/* enable blending */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	/* enable backface culling */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_DEPTH_TEST);

	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, width, height, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	ilInit();
	if(!quadlist)
	{
		quadlist = glGenLists(1);
		glNewList(quadlist, GL_COMPILE);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(0, 0);
		glTexCoord2f(0, 1); glVertex2f(0, 1);
		glTexCoord2f(1, 1); glVertex2f(1, 1);
		glTexCoord2f(1, 0); glVertex2f(1, 0);
		glEnd();
		glEndList();
	}
	return 0;
}

int Lua_Graphics_getWindowWidth(lua_State *L) {
	lua_pushinteger(L, screen->w);
	return 1;
}

int Lua_Graphics_getWindowHeight(lua_State *L) {
	lua_pushinteger(L, screen->h);
	return 1;
}

int Lua_Graphics_getWindowSize(lua_State *L) {
	lua_pushinteger(L, screen->w);
	lua_pushinteger(L, screen->h);
	return 2;
}

int Lua_Graphics_getModes(lua_State *L) {
	SDL_Rect ** modes = SDL_ListModes(0, SDL_OPENGL | SDL_FULLSCREEN);
	int i, index = 1;
	if(modes == (SDL_Rect **)0 || modes == (SDL_Rect **)-1)
		return 0;

	lua_newtable(L);
	
	for(i=0; modes[i]; i++)
	{
		lua_pushinteger(L, index);
		lua_newtable(L);
		lua_pushstring(L, "width");
		lua_pushinteger(L, modes[i]->w);
		lua_settable(L, -3);
		lua_pushstring(L, "height");
		lua_pushinteger(L, modes[i]->h);
		lua_settable(L, -3);
		lua_settable(L, -3);
		index++;
	}
	return 1;
}
