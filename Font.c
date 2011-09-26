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

#define checkfont(L) (Lua_Font *)luaL_checkudata(L, 1, "scrupp.font")
#define checktext(L) (Lua_Text *)luaL_checkudata(L, 1, "scrupp.vbotext")

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
    lua_pushinteger(L, font->height * font->scale);
	return 1;
}

static int Lua_Font_stringWidth(lua_State *L) {
	const char * str;
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    str = luaL_checkstring(L, 1);
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
	register const char * str;
	const char *align;
	float x, y, maxw, w;
	unsigned char c;
    Lua_FontChar *ch;
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    str = luaL_checkstring(L, 1);
    x = (float)luaL_checknumber(L, 2);
    y = (float)luaL_checknumber(L, 3);
    maxw = (float)lua_tonumber(L, 4);
    align = lua_tostring(L, 5);
    w = 0;
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
                continue;
	    }
        ch = &currentFont->chars[(unsigned char)*str];
	DRAW_CHAR
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
	float x, y, maxw;
	register const char * str;
	const char *align;
    Lua_FontChar *ch;
	int i = 0, last_space = 0, buf = 0;
	float w = 0;
	unsigned char c;
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    str = luaL_checkstring(L, 1);
    x = (float)luaL_checknumber(L, 2);
    y = (float)luaL_checknumber(L, 3);
    maxw = (float)luaL_checknumber(L, 4) / currentFont->scale;
    align = lua_tostring(L, 5);
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
	register char * str;
	float maxw;
	int j = 1;
	int i = 0, last_space = 0, buf = 0;
	float w = 0;
	unsigned char c;
    if(!currentFont) return luaL_error(L, "Call <yourfont>:select() first!");
    str = (char *)luaL_checkstring(L, 1);
    maxw = (float)luaL_checknumber(L, 2) / currentFont->scale;
    lua_newtable(L);
    

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
	//~ unsigned short x,y,w,h,cx,cy;
	
	unsigned int ch = (unsigned int)luaL_checkint(L, 2);
	float x1 = (float)luaL_checknumber(L, 3) / ptr->w;
	float y1 = (float)luaL_checknumber(L, 4) / ptr->h;
	float x2 = x1 + (float)luaL_checknumber(L, 5) / ptr->w;
	float y2 = y1 + (float)luaL_checknumber(L, 6) / ptr->h;
	float cx1 = (float)luaL_checknumber(L, 7);
	float cy1 = (float)luaL_checknumber(L, 8);
	float cx2 = cx1 + (float)luaL_checknumber(L, 5);
	float cy2 = cy1 + (float)luaL_checknumber(L, 6);
	#ifndef NO_VBO
	float vert[] = {cx1,cy1,cx1,cy2,cx2,cy2,cx2,cy1};
	float tex[] = {x1,y1,x1,y2,x2,y2,x2,y1};
	#endif
	ptr->chars[ch].x = luaL_checkinteger(L, 3);
	ptr->chars[ch].y = luaL_checkinteger(L, 4);
	ptr->chars[ch].cw = luaL_checkinteger(L, 5);
	ptr->chars[ch].ch = luaL_checkinteger(L, 6);
	ptr->chars[ch].cx = luaL_checkinteger(L, 7);
	ptr->chars[ch].cy = luaL_checkinteger(L, 8);
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
	int i;
	Lua_Font * font = checkfont(L);
	
	for(i = 0; i< 255; i++)
		if(font->chars[i].vertex) glDeleteLists(font->chars[i].vertex, 1);
	
	return 0;
}

static int font_tostring(lua_State *L) {
	lua_pushliteral(L, "Font");
	return 1;
}

void Lua_Text_genvbo(Lua_Text *ptr, const char * text, unsigned int l)
{
	float * vert;
	float * tex;
	float x, y, w, h, x1, y1, x2, y2, cx1, cy1, cx2, cy2;
	unsigned int i, j, last_space;
	unsigned char c;
	Lua_FontChar * ch;
	vert = (float *)malloc(sizeof(float) * l * 8);
	tex = (float *)malloc(sizeof(float) * l * 8);
	j = 0;
	y = x = 0.0f;
	w = ptr->font->w;
	h = ptr->font->h;
	ptr->l = l*4;
	for(i=0;i<l;i++)
	{
		c = (unsigned char)text[i];
    switch(c)
    {
			case '\t':
				x += ptr->font->chars[32].w * 8;
				last_space = i;
				ptr->l -= 4;
				continue;
			case ' ':
				x += ptr->font->chars[32].w;
				last_space = i;
				ptr->l -= 4;
				continue;
			case '\n':
				y += ptr->font->height;
				x = 0;
				ptr->l -= 4;
				continue;
    }
		ch = &ptr->font->chars[c];
		x1 = (float)ch->x / w;
		y1 = (float)ch->y / h;
		x2 = x1 + (float)ch->cw / w;
		y2 = y1 + (float)ch->ch / h;
		cx1 = x + (float)ch->cx;
		cy1 = y + (float)ch->cy;
		cx2 = cx1 + (float)ch->cw;
		cy2 = cy1 + (float)ch->ch;
		//~ printf("%f %f %f %f %f %f %f %f\n", x1, y1, x2, y2, cx1, cy1, cx2, cy2);
		//~ printf("%d %d %d %d %f %f %f %f\n", ch->x, ch->y, ch->cw, ch->ch, w, h, cx2, cy2);
		tex[j] = x1;		vert[j] = cx1;		j++;
		tex[j] = y1;		vert[j] = cy1;		j++;
		tex[j] = x1;		vert[j] = cx1;		j++;
		tex[j] = y2;		vert[j] = cy2;		j++;
		
		tex[j] = x2;		vert[j] = cx2;		j++;
		tex[j] = y2;		vert[j] = cy2;		j++;
		tex[j] = x2;		vert[j] = cx2;		j++;
		tex[j] = y1;		vert[j] = cy1;		j++;
		
		x += ch->w;
	}
	if(ptr->v) glDeleteBuffersARB(1, &ptr->v);
	if(ptr->t) glDeleteBuffersARB(1, &ptr->t);
	glGenBuffersARB(1, &ptr->v);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, ptr->v);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * l * 8, vert, GL_STATIC_DRAW_ARB);
	glGenBuffersARB( 1, &ptr->t );
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, ptr->t);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float) * l * 8, tex, GL_STATIC_DRAW_ARB);
	free(vert);
	free(tex);
}

static int Lua_Text_new(lua_State *L) {
	unsigned int l;
	Lua_Font * font;
	const char * text;
	Lua_Text *ptr;
	font = checkfont(L);
	text = luaL_checklstring(L, 2, &l);
	ptr = lua_newuserdata(L, sizeof(Lua_Text));
	ptr->font = font;
	ptr->v = 0;
	ptr->t = 0;
	//~ ptr->text = (char*)malloc(sizeof(char)*l+1);
	//~ strcpy(ptr->text, text);
	Lua_Text_genvbo(ptr, text, l);
	luaL_getmetatable(L, "scrupp.vbotext");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Text_setText(lua_State *L) {
	Lua_Text *ptr;
	unsigned int l;
	const char * text;
	
	ptr = checktext(L);
	text = luaL_checklstring(L, 2, &l);
	Lua_Text_genvbo(ptr, text, l);
	return 0;
}

static int Lua_Text_draw(lua_State *L) {
	Lua_Text *ptr;
	double x, y;
	ptr = checktext(L);
	x = lua_tonumber(L, 2);
	y = lua_tonumber(L, 3);
	glPushMatrix();
	glTranslated(x,y,0);
	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ptr->font->texture);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, ptr->v);
	glVertexPointer(2, GL_FLOAT, 0, (char *) NULL);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, ptr->t);
	glTexCoordPointer(2, GL_FLOAT, 0, (char *) NULL);
	glDrawArrays(GL_QUADS, 0, ptr->l);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
	return 0;
}

static int text_gc(lua_State *L) {
	//~ Lua_Text * text = checktext(L);
	//~ if(text->text) free(text->text);
	return 0;
}

static int text_tostring(lua_State *L) {
	lua_pushliteral(L, "Text");
	return 1;
}
static const struct luaL_Reg fontlib [] = {
	{"addFont", Lua_Font_load},
	{"print", Lua_Font_print},
	{"printf", Lua_Font_printf},
	{"stringWidth", Lua_Font_stringWidth},
	{"stringHeight", Lua_Font_stringHeight},
	{"stringToLines", Lua_Font_stringToLines},
	{"newText", Lua_Text_new},

	{NULL, NULL}
};

static const struct luaL_Reg textlib_m [] = {
	{"__gc", text_gc},
	{"__tostring", text_tostring},
	{"setText", Lua_Text_setText},
	//~ {"setAlign", Lua_Text_setAlign},
	//~ {"setWidth", Lua_Text_setWidth},
	{"draw", Lua_Text_draw},
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
	{"newText", Lua_Text_new},
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
	
	luaL_newmetatable(L, "scrupp.vbotext");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, textlib_m);
	lua_pop(L, 1);	/* pop the metatable */
	
	luaL_register(L, parent, fontlib);
	return 1;
}
