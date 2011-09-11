#include <math.h>



#include "../SOIL/SOIL.h"
#include "../Main.h"
#include "../Macros.h"
#include "Graphics.h"
#include "../physfsrwops.h"
#include "../render.h"

#define GL_LUA_GET_POINT    GLdouble x1 = (GLdouble)luaL_checknumber(L, 1); \
                            GLdouble y1 = (GLdouble)luaL_checknumber(L, 2);

int Lua_Graphics_line(lua_State *L) {
    GL_LUA_GET_POINT
    GLdouble x2 = (GLdouble)luaL_checknumber(L, 3);
    GLdouble y2 = (GLdouble)luaL_checknumber(L, 4);
    glBegin(GL_LINES);
    glVertex2d(x1, y1);
    glVertex2d(x2, y2);
    glEnd();
    return 0;
}

int Lua_Graphics_rectangle(lua_State *L) {
    GL_LUA_GET_POINT
    GLdouble x2 = (GLdouble)luaL_checknumber(L, 3) + x1;
    GLdouble y2 = (GLdouble)luaL_checknumber(L, 4) + y1;
    char filled = lua_toboolean(L, 5);
    glBegin(filled?GL_QUADS:GL_LINE_LOOP);
    glVertex2d(x1, y1);
    glVertex2d(x2, y1);
    glVertex2d(x2, y2);
    glVertex2d(x1, y2);
    glEnd();
    return 0;
}

int Lua_Graphics_circle(lua_State *L) {
    GL_LUA_GET_POINT
    GLdouble rad = (GLdouble)luaL_checknumber(L, 3);
    int segments = luaL_checkint(L, 4);
    char filled = lua_toboolean(L, 5);
    int i;
    const double DBLPI = 3.1415926 * 2;
    GLdouble angle;
    glBegin(filled?GL_TRIANGLE_FAN:GL_LINE_LOOP);
    for (i = 0; i <= segments; i++)
    {
        angle = DBLPI / (double)segments * (double)i;
        glVertex2d(x1 + sin(angle) * rad, y1 + cos(angle) * rad);
    }
    glEnd();
    return 0;
}

int Lua_Graphics_point(lua_State *L) {
    GL_LUA_GET_POINT
    glBegin(GL_POINTS);
    glVertex2d(x1, y1);
    glEnd();
    return 0;
}

int Lua_Graphics_setPointSize(lua_State *L) {
    glPointSize(luaL_checknumber(L, 1));
    return 0;
}

int Lua_Graphics_setLineWidth(lua_State *L) {
    glLineWidth((GLfloat)luaL_checknumber(L, 1));
    return 0;
}

int Lua_Graphics_setSmooth(lua_State *L) {
    if(lua_toboolean(L, 1))
    {
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
    }
    else
    {
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
    }
    return 0;
}

#define LUA_GL_SETCOLOR(var, num) lua_pushinteger(L, num);\
        lua_gettable(L, -2);\
        var = (GLubyte)luaL_checkint(L, -1);\
        lua_pop(L, 1);

int Lua_Graphics_setColor(lua_State *L) {
    GLubyte r,g,b,a;
    if (lua_istable(L, 1)) {
        LUA_GL_SETCOLOR(r, 1)
        LUA_GL_SETCOLOR(g, 2)
        LUA_GL_SETCOLOR(b, 3)
        lua_pushinteger(L, 4);
        lua_gettable(L, -2);
        a = (GLubyte)luaL_optint(L, -1, 255);
        lua_pop(L, 1);
    }
    else
    {
        r = (GLubyte)luaL_checkint(L, 1);
        g = (GLubyte)luaL_checkint(L, 2);
        b = (GLubyte)luaL_checkint(L, 3);
        a = (GLubyte)luaL_optint(L, 4, 255);
    }
	glColor4ub(r,g,b,a);
	return 0;
}

int Lua_Graphics_setBlendMode(lua_State *L) {
	const char * str = luaL_checkstring(L, 1);
	if(strcmp(str, "subtractive") == 0)
  {
    glBlendEquation_(GL_FUNC_REVERSE_SUBTRACT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
	else
    glBlendEquation_(GL_FUNC_ADD);
	if (strcmp(str, "alpha") == 0)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else if (strcmp(str, "multiplicative") == 0)
		glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	else if (strcmp(str, "additive") == 0)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  else if (strcmp(str, "screen") == 0)
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
  //~ else if (strcmp(str, "mask") == 0)
    //~ glBlendFuncSeparate_(GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ZERO);
  //~ else if (strcmp(str, "foreground") == 0)
    //~ glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	return 0;
}
