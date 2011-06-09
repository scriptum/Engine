/*
** $Id: Font.h 10 2008-07-01 22:18:32Z basique $
** Library to use TrueType fonts with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#ifndef __FONT_H__
#define __FONT_H__

#include <SDL_opengl.h>

typedef struct Lua_FontChar
{
    GLuint list;
    float w, h;
} Lua_FontChar;

typedef struct Lua_Font {
	/* OpenGL texture id */
	GLuint texture;
	float w, h;
	Lua_FontChar chars[256];
} Lua_Font;

int luaopen_font(lua_State *L, const char *parent);



#endif /* __FONT_H__ */
