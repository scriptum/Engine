/*
** $Id: Font.c 45 2009-03-06 23:45:34Z basique $
** Library to use TrueType fonts with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Macros.h"
#include "Font.h"
#include "Graphics/Graphics.h"
#include "physfsrwops.h"

#define checkfont(L) \
	(Lua_Font *)luaL_checkudata(L, 1, "scrupp.font")

Lua_Font * currentFont;

static int Lua_Font_load(lua_State *L) {
    Lua_Image *img = (Lua_Image *)luaL_checkudata(L, 1, "scrupp.image");
	Lua_Font *ptr;
	ptr = lua_newuserdata(L, sizeof(Lua_Font));
	ptr->texture = img->texture;
	//printf("%d\n",img->texture);
	ptr->w = img->w;
	ptr->h = img->h;
	luaL_getmetatable(L, "scrupp.font");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Font_print(lua_State *L) {
    if(!currentFont) return luaL_error(L, "Call Font.set(<font name>, <font size>) first!");
    const char * str = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float w = 0, buf1, buf2;
    unsigned char c;
    Lua_FontChar *ch;
    glPushMatrix();
    glTranslatef(x, y, 0);

    glBindTexture(GL_TEXTURE_2D, currentFont->texture);
    glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
    //printf("%d\n",currentFont->texture);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_ALPHA_TEST);
    if(*str)
	do {
	    if(*str == '\n') {
	        glTranslatef(-w, currentFont->chars[32].h, 0);
	        w = 0;
	        continue;
        } else if(*str == '\t')
        {
            glTranslatef(currentFont->chars[32].w * 8, 0, 0);
            w += currentFont->chars[32].w * 8;
	        continue;
        }
        ch = &currentFont->chars[(unsigned char)*str];
        glCallList(ch->list);
        glTranslatef(ch->w, 0, 0);
        w += ch->w;
	} while(*++str);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPopMatrix();
    glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	return 0;
}

static int Lua_Font_width(lua_State *L) {
    int width = 0;
    Lua_Font *font = checkfont(L);
    const char * str = luaL_checkstring(L, 2);
    if(*str)
	do {
	    if(*str == '\t')
	    {
            width += font->chars[32].w * 8;
	        continue;
        }
	    width += font->chars[(unsigned char)*str].w;
	} while(*++str);
	lua_pushinteger(L, width);
	return 1;
}

static int Lua_Font_height(lua_State *L) {
    Lua_Font *font = checkfont(L);
    lua_pushinteger(L, font->chars[32].h);
	return 1;
}

static int Lua_Font_stringWidth(lua_State *L) {
    int width = 0;
    const char * str = luaL_checkstring(L, 1);
    if(*str)
	do {
	    if(*str == '\t')
	    {
            width += currentFont->chars[32].w * 8;
	        continue;
        }
	    width += currentFont->chars[(unsigned char)*str].w;
	} while(*++str);
	lua_pushinteger(L, width);
	return 1;
}

static int Lua_Font_stringHeight(lua_State *L) {
    lua_pushinteger(L, currentFont->chars[32].h);
	return 1;
}

static int Lua_Font_setCurrentFont(lua_State *L) {
    currentFont = checkfont(L);
    return 0;
}

static int Lua_Font_setGlyph(lua_State *L) {
	Lua_Font *ptr = checkfont(L);
	unsigned int ch = (unsigned int)luaL_checkint(L, 2);
	float x1 = (float)luaL_checknumber(L, 3) / ptr->w;
	float y1 = (float)luaL_checknumber(L, 4) / ptr->h;
	float x2 = x1 + (float)luaL_checknumber(L, 5) / ptr->w;
	float y2 = y1 + (float)luaL_checknumber(L, 6) / ptr->h;
	float cx1 = (float)luaL_checknumber(L, 7);
	float cy1 = (float)luaL_checknumber(L, 8);
	float cx2 = cx1 + (float)luaL_checknumber(L, 5);
	float cy2 = cy1 + (float)luaL_checknumber(L, 6);
	ptr->chars[ch].list = glGenLists(1);
	glNewList(ptr->chars[ch].list,GL_COMPILE);
	glBegin(GL_QUADS);
    glTexCoord2f(x1, y1);         glVertex2i(cx1, cy1);
    glTexCoord2f(x1, y2);         glVertex2i(cx1, cy2);
    glTexCoord2f(x2, y2);         glVertex2i(cx2, cy2);
    glTexCoord2f(x2, y1);         glVertex2i(cx2, cy1);
    glEnd();
    glEndList();
	ptr->chars[ch].w = (float)luaL_checknumber(L, 9);
	ptr->chars[ch].h = (float)luaL_checknumber(L, 10);
	return 0;
}

static int font_gc(lua_State *L) {
	return 0;
}

static int font_tostring(lua_State *L) {
	lua_pushliteral(L, "Font");
	return 1;
}

static const struct luaL_Reg fontlib [] = {
	{"addFont", Lua_Font_load},
	{"print", Lua_Font_print},
	{"stringWidth", Lua_Font_stringWidth},
	{"stringHeight", Lua_Font_stringHeight},
	{NULL, NULL}
};

static const struct luaL_Reg fontlib_m [] = {
	{"__gc", font_gc},
	{"__tostring", font_tostring},
	{"setGlyph", Lua_Font_setGlyph},
	{"select", Lua_Font_setCurrentFont},
	{"width", Lua_Font_width},
	{"height", Lua_Font_height},
	{NULL, NULL}
};

int luaopen_font(lua_State *L, const char *parent) {
    currentFont = NULL;
	luaL_newmetatable(L, "scrupp.font");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, fontlib_m);
	lua_pop(L, 1);	/* pop the metatable */
	luaL_register(L, parent, fontlib);
	return 1;
}
