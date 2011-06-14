/*
** $Id: Graphics.h 56 2009-09-03 21:43:48Z basique $
** Graphic-related functions and library to use them with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <SDL_opengl.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	#define RMASK 0xFF000000
	#define GMASK 0x00FF0000
	#define BMASK 0x0000FF00
	#define AMASK 0x000000FF
#else
	#define RMASK 0x000000FF
	#define GMASK 0x0000FF00
	#define BMASK 0x00FF0000
	#define AMASK 0xFF000000
#endif

typedef struct Lua_Image {
	/* OpenGL texture id */
	GLuint texture, list;
	/* width and height of the original image */
	int w;
	int h;
} Lua_Image;

int Lua_Graphics_init(lua_State *L);

int Lua_Graphics_showCursor(lua_State *L);

int Lua_Graphics_getWindowWidth(lua_State *L);

int Lua_Graphics_getWindowHeight(lua_State *L);

int Lua_Graphics_getWindowSize(lua_State *L);

int Lua_Graphics_getTicks(lua_State *L);

int Lua_Graphics_translateView(lua_State *L);

int Lua_Graphics_scaleView(lua_State *L);

int Lua_Graphics_setColor(lua_State *L);

int Lua_Graphics_setBlendMode(lua_State *L);

int Lua_Graphics_rotateView(lua_State *L);

int Lua_Graphics_saveView(lua_State *L);

int Lua_Graphics_restoreView(lua_State *L);

int Lua_Graphics_resetView(lua_State *L);

int load_image_from_string(lua_State *L, const unsigned char *myBuf, unsigned int file_size, int repeat);

int Lua_Image_load(lua_State *L);

int Lua_Image_loadFromString(lua_State *L);

int Lua_Image_getWidth(lua_State *L);

int Lua_Image_getHeight(lua_State *L);

int Lua_Image_getTextureId(lua_State *L);

int Lua_Image_getSize(lua_State *L);

int Lua_Image_draw(lua_State *L);

int Lua_Image_drawq(lua_State *L);

int image_gc(lua_State *L);

int image_tostring(lua_State *L);

int Lua_Graphics_line(lua_State *L);

int Lua_Graphics_rectangle(lua_State *L);

int Lua_Graphics_setLineWidth(lua_State *L);

int Lua_Graphics_setSmooth(lua_State *L);

int Lua_Graphics_setPointSize(lua_State *L);

int Lua_Graphics_circle(lua_State *L);

int Lua_Graphics_point(lua_State *L);

int luaopen_graphics(lua_State *L, const char *parent);

#endif /* __GRAPHICS_H__ */
