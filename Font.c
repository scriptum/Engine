/*
** $Id: Font.c 45 2009-03-06 23:45:34Z basique $
** Library to use TrueType fonts with Scrupp
** See Copyright Notice in COPYRIGHT
*/
#include "math.h"

#include "Main.h"
#include "Macros.h"
#include "Font.h"
#include "Graphics/Graphics.h"
#include "physfs.h"

#define checkfont(L) \
	(Lua_Font *)luaL_checkudata(L, 1, "scrupp.font")

#define NO_VBO

Lua_Font * currentFont;

static int Lua_Font_load(lua_State *L) {
    Lua_Image *img = (Lua_Image *)luaL_checkudata(L, 1, "scrupp.image");
    Lua_Font *ptr;
    ptr = lua_newuserdata(L, sizeof(Lua_Font));
    ptr->texture = img->texture;
    ptr->scale = 1;
    //printf("%d\n",img->texture);
    ptr->w = img->w;
    ptr->h = img->h;
    luaL_getmetatable(L, "scrupp.font");
    lua_setmetatable(L, -2);
    return 1;
}

float Lua_Font_Width(Lua_Font *f, register const char *str)
{
    float width = 0;
    if(*str)
	do {
	    if(*str == '\t')\
	    {
            width += f->chars[32].w * 8;
	        continue;
        }
        else if(*str == '\n')
            width = 0;
	    width += f->chars[(unsigned char)*str].w;
	} while(*++str);

	return width * f->scale;
}

static int Lua_Font_width(lua_State *L) {
    Lua_Font *font = checkfont(L);
    const char * str = luaL_checkstring(L, 2);
    lua_pushnumber(L, Lua_Font_Width(font, str));
    return 1;
}

static int Lua_Font_height(lua_State *L) {
    Lua_Font *font = checkfont(L);
    lua_pushinteger(L, font->height);
	return 1;
}

static int Lua_Font_stringWidth(lua_State *L) {
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    const char * str = luaL_checkstring(L, 1);
    lua_pushnumber(L, Lua_Font_Width(currentFont, str));
    return 1;
}

static int Lua_Font_stringHeight(lua_State *L) {
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    lua_pushinteger(L, currentFont->height);
	return 1;
}

#define ALIGN(width) if(align)\
    if(strcmp(align, "center") == 0) glTranslatef(floor((maxw - width)/2.0f), 0, 0);\
    else if(strcmp(align, "right") == 0) glTranslatef(floor(maxw - width), 0, 0);
#ifdef NO_VBO
#define DRAW_CHAR glCallList(ch->vertex);
#else
#define DRAW_CHAR \
glBindBufferARB(GL_ARRAY_BUFFER_ARB, ch->vertex); \
glVertexPointer(2, GL_FLOAT, 0, (char *) NULL); \
glBindBufferARB(GL_ARRAY_BUFFER_ARB, ch->tex); \
glTexCoordPointer(2, GL_FLOAT, 0, (char *) NULL); \
glDrawArrays(GL_QUADS, 0, 4); 
#endif

static int Lua_Font_print(lua_State *L) {
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    register const char * str = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float maxw = (float)lua_tonumber(L, 4);
    const char *align = lua_tostring(L, 5);
    float w = 0;
    unsigned char c;
    Lua_FontChar *ch;
    glPushMatrix();
    glTranslatef(x, y, 0);

    ALIGN(Lua_Font_Width(currentFont, str))

    glScalef(currentFont->scale, currentFont->scale, 0);
    glBindTexture(GL_TEXTURE_2D, currentFont->texture);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    #ifndef NO_VBO
    glEnableClientState(GL_VERTEX_ARRAY); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
    #endif
    if(*str)
	do {
	    switch(*str)
	    {
	        case '\n':
                glTranslatef(-w, currentFont->height, 0);
                w = 0;
                continue;
            case '\t':
                glTranslatef(currentFont->chars[32].w * 8, 0, 0);
                w += currentFont->chars[32].w * 8;
//                last_space = str;
                continue;
//            case ' ':
//                last_space = str;
	    }
        ch = &currentFont->chars[(unsigned char)*str];
	DRAW_CHAR
        //~ glCallList(ch->list);
        glTranslatef(ch->w, 0, 0);
        w += ch->w;
	} while(*++str);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPopMatrix();
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    #ifndef NO_VBO
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    #endif
    return 0;
}

#define PRINT_LINES(A,B) \
int i = 0, last_space = 0, buf = 0;\
float w = 0;\
unsigned char c;\
while(str[i]) {\
    c = (unsigned char)str[i];\
    switch(c)\
    {\
        case '\t':\
            w += currentFont->chars[32].w * 8;\
            last_space = i;\
            break;\
        case ' ':\
            last_space = i;\
        default:\
            w += currentFont->chars[c].w;\
    }\
    if(w > maxw || c == '\n')\
    {\
        if(!last_space || c == '\n') last_space = i;\
        A\
        i = last_space;\
        buf = last_space + 1;\
        last_space = 0;\
        w = 0;\
    }\
    i++;\
}\
B

static int Lua_Font_printf(lua_State *L) {
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    register const char * str = luaL_checkstring(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float maxw = (float)luaL_checknumber(L, 4) / currentFont->scale;
    const char *align = lua_tostring(L, 5);
    Lua_FontChar *ch;
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(currentFont->scale, currentFont->scale, 0);
    glBindTexture(GL_TEXTURE_2D, currentFont->texture);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    #ifndef NO_VBO
    glEnableClientState(GL_VERTEX_ARRAY); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    #endif

    PRINT_LINES(
    ALIGN(w)
    w = 0;
    while(buf < last_space) {
        c = (unsigned char)str[buf];
        if(c == '\t')
        {
            glTranslatef(currentFont->chars[32].w * 8, 0, 0);
            w += currentFont->chars[32].w * 8;
            continue;
        }
        ch = &currentFont->chars[c];
	DRAW_CHAR
        glTranslatef(ch->w, 0, 0);
        w += ch->w;
        buf++;
    }
    glTranslatef(-w, currentFont->height, 0);
    ,
    ALIGN(w)
	while(buf < i) {
        c = (unsigned char)str[buf];
        if(c == '\t')
        {
            glTranslatef(currentFont->chars[32].w * 8, 0, 0);
            continue;
        }
        ch = &currentFont->chars[c];
	DRAW_CHAR
        glTranslatef(ch->w, 0, 0);
        buf++;
    })

    glPopMatrix();
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    #ifndef NO_VBO
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    #endif
    return 0;
}

static int Lua_Font_stringToLines(lua_State *L) {
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    register char * str = (char *)luaL_checkstring(L, 1);
    float maxw = (float)luaL_checknumber(L, 2) / currentFont->scale;
    lua_newtable(L);
    int j = 1;

    PRINT_LINES(lua_pushlstring(L, str + buf, last_space - buf);
            lua_rawseti(L, -2, j++);,
            lua_pushstring(L, str + buf);
            lua_rawseti(L, -2, j);)

    return 1;
}

static int Lua_Font_setCurrentFont(lua_State *L) {
    currentFont = checkfont(L);
    return 0;
}

static int Lua_Font_scale(lua_State *L) {
    Lua_Font *font = checkfont(L);
    font->scale = luaL_checknumber(L, 2);
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
	float vert[] = {cx1,cy1,cx1,cy2,cx2,cy2,cx2,cy1};
	float tex[] = {x1,y1,x1,y2,x2,y2,x2,y1};
	#ifdef NO_VBO
	ptr->chars[ch].vertex = glGenLists(1);
	glNewList(ptr->chars[ch].vertex, GL_COMPILE);
	glBegin(GL_QUADS);
	glTexCoord2f(x1, y1);         glVertex2i(cx1, cy1);
	glTexCoord2f(x1, y2);         glVertex2i(cx1, cy2);
	glTexCoord2f(x2, y2);         glVertex2i(cx2, cy2);
	glTexCoord2f(x2, y1);         glVertex2i(cx2, cy1);
	glEnd();
	glEndList();
	#else
	glGenBuffersARB(1, &ptr->chars[ch].vertex);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, ptr->chars[ch].vertex);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vert), vert, GL_STATIC_DRAW_ARB);
	glGenBuffersARB( 1, &ptr->chars[ch].tex );
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, ptr->chars[ch].tex);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(tex), tex, GL_STATIC_DRAW_ARB);
	#endif

	ptr->chars[ch].w = (float)luaL_checknumber(L, 9);
	ptr->height = (float)luaL_checknumber(L, 10);
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
	{"printf", Lua_Font_printf},
	{"stringWidth", Lua_Font_stringWidth},
	{"stringHeight", Lua_Font_stringHeight},
	{"stringToLines", Lua_Font_stringToLines},

	{NULL, NULL}
};

static const struct luaL_Reg fontlib_m [] = {
	{"__gc", font_gc},
	{"__tostring", font_tostring},
	{"setGlyph", Lua_Font_setGlyph},
	{"select", Lua_Font_setCurrentFont},
	{"width", Lua_Font_width},
	{"height", Lua_Font_height},
	{"scale", Lua_Font_scale},
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
