/*
** $Id: Sound.h 45 2009-03-06 23:45:34Z basique $
** Library for sound and music with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#ifndef __SOUND_H__
#define __SOUND_H__

#include "SDL_mixer.h"

typedef struct Lua_Sound {
	Mix_Chunk 	*sample;
	int			channel;
} Lua_Sound;

int luaopen_sound(lua_State *L, const char *parent);

#endif /* __SOUND_H__ */
