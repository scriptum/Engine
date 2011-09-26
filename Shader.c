#include "Main.h"
#include "Macros.h"
#include "Shader.h"
#include "physfs.h"
#include "render.h"
char * loadfile(const char * name, unsigned int * length)
{
	char * data;
	unsigned int len;
	PHYSFS_file* myfile = PHYSFS_openRead(name);
	if (!myfile)
		return 0;
	len = PHYSFS_fileLength(myfile);
	*length = len;
	data = (char *)malloc(sizeof(char) * len + 1);
	PHYSFS_read (myfile, data, sizeof(char), len);
	PHYSFS_close(myfile);
	data[len] = 0;
	return data;
}

int compile(GLuint shader, const char* name)
{
	GLint compiled;
	GLint blen = 0;
	GLsizei slen = 0;
	GLchar* compiler_log;
	
	glCompileShader_(shader);

	glGetShaderiv_(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		glGetShaderiv_(shader, GL_INFO_LOG_LENGTH , &blen);       
		if (blen > 1)
		{
			compiler_log = (GLchar*)malloc(blen);
			glGetInfoLog_(shader, blen, &slen, compiler_log);
			printf("Error while compiling shader %s: %s\n", name, compiler_log);
			free (compiler_log);
		}
		return 0;
	}
	return 1;
}

static int Lua_Shader_load(lua_State *L)
{
	char *vs,*fs;
	const char* vsname = luaL_checkstring(L, 1); //vertex shader
	const char* fsname = luaL_checkstring(L, 2); //fragment shader
	GLuint v, f, p;
	unsigned int vslen, fslen;
	GLint linked;
	if(!supported.GLSL) return 0;
	//create shader
	v = glCreateShaderObject_(GL_VERTEX_SHADER);
	f = glCreateShaderObject_(GL_FRAGMENT_SHADER);

	//loading shader source
	vs = loadfile(vsname, &vslen);
	fs = loadfile(fsname, &fslen);
	glShaderSource_(v, 1, &vs, NULL);
	glShaderSource_(f, 1, &fs, NULL);
	free(vs);
	free(fs);
	if(!compile(v, vsname))
	{
		lua_pushnil(L);
		return 1;
	}
	if(!compile(f, fsname))
	{
		lua_pushnil(L);
		return 1;
	}
	p = glCreateProgramObject_();
	glAttachObject_(p,v);
	glAttachObject_(p,f);
	glLinkProgram_(p);
  
	glGetProgramiv_(p, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		printf("Error while linking shader %s %s\n", vsname, fsname);
		lua_pushnil(L);
		return 1;
	}
	lua_pushinteger(L, p);
	return 1;
}

GLuint current_program;
static int Lua_Shader_use(lua_State *L)
{
	if(!supported.GLSL) return 0;
	current_program = (GLuint)luaL_checkint(L, 1);
	glUseProgramObject_(current_program);
}

GLuint getUniform(lua_State *L)
{
	const char *name;
	GLuint location;
	if(!supported.GLSL) return 0;
	if(lua_isnumber(L, 1)) return luaL_checkint(L, 1);
	name = lua_tostring(L, 1);
	location = glGetUniformLocation_(current_program, name);
	//~ if(location == -1) return luaL_error(L, "Error: uniform '%s' not found", name);
	return location;
}

static int Lua_Shader_setUniformf(lua_State *L)
{
	if(!supported.GLSL) return 0;
	switch(lua_gettop(L))
	{
		case 2:
			glUniform1f_(getUniform(L), (GLfloat)luaL_checknumber(L, 2));
			break;
		case 3:
			glUniform2f_(getUniform(L), (GLfloat)luaL_checknumber(L, 2), (GLfloat)luaL_checknumber(L, 3));
			break;
		case 4:
			glUniform3f_(getUniform(L), (GLfloat)luaL_checknumber(L, 2), (GLfloat)luaL_checknumber(L, 3), (GLfloat)luaL_checknumber(L, 4));
			break;
		case 5:
			glUniform4f_(getUniform(L), (GLfloat)luaL_checknumber(L, 2), (GLfloat)luaL_checknumber(L, 3), (GLfloat)luaL_checknumber(L, 4), (GLfloat)luaL_checknumber(L, 5));
			break;
	}
	return 0;
}

static int Lua_Shader_setUniformi(lua_State *L)
{
	if(!supported.GLSL) return 0;
	glUniform1i_(getUniform(L), (int)luaL_checknumber(L, 2));
	return 0;
}
static const struct luaL_Reg shaderlib [] = {
	{"newShader", Lua_Shader_load},
	{"useShader", Lua_Shader_use},
	{"setUniformf", Lua_Shader_setUniformf},
	{"setUniformi", Lua_Shader_setUniformi},
	{NULL, NULL}
};


int luaopen_shader(lua_State *L, const char *parent) {
	luaL_register(L, parent, shaderlib);
	return 1;
}
