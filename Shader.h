#ifndef __SHADER_H__
#define __SHADER_H__

#include <SDL_opengl.h>
//#define GL_GLEXT_PROTOTYPES
typedef struct Lua_Shader
{
    GLuint program;
} Lua_Shader;



int luaopen_shader(lua_State *L, const char *parent);



#endif /* __SHADER_H__ */