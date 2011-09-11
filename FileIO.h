/*
** $Id: FileIO.h 45 2009-03-06 23:45:34Z basique $
** File I/O: Access to the virtual file system
** See Copyright Notice in COPYRIGHT
*/

#ifndef __FILEIO_H__
#define __FILEIO_H__

//~ #ifdef LOVE_WINDOWS
//~ #	include <windows.h>
//~ #	include <direct.h>
//~ #else
//~ #	include <sys/param.h>
//~ #	include <unistd.h>
//~ #endif

#define FILEIO_SUCCESS 0
#define FILEIO_ERROR 1

void FS_Init(lua_State *L, char *argv[], char **filename);
int FS_loadFile(lua_State *L, const char *filename);

int luaopen_fileio(lua_State *L, const char *parent);

#endif /* __FILEIO_H__ */
