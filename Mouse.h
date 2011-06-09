/*
** $Id: Mouse.h 10 2008-07-01 22:18:32Z basique $
** Library to use the mouse with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#ifndef __MOUSE_H__
#define __MOUSE_H__

extern const char* buttonNames [];

int luaopen_mouse(lua_State *L, const char *parent);

#endif /* __MOUSE_H__ */
