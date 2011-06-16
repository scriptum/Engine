#include "Main.h"
#include "Macros.h"
#include "Shader.h"
#include "physfs.h"

char * loadfile(const char * name, unsigned int * length)
{
	char * data;
	PHYSFS_file* myfile = PHYSFS_openRead(name);
	if (!myfile)
		return 0;
	unsigned int len;
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
	glCompileShader(shader);
	GLint compiled;
	GLint blen = 0;
	GLsizei slen = 0;
	GLchar* compiler_log;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &blen);       
		if (blen > 1)
		{
			compiler_log = (GLchar*)malloc(blen);
			glGetInfoLogARB(shader, blen, &slen, compiler_log);
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
	//create shader
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	unsigned int vslen, fslen;
	//loading shader source
	vs = loadfile(vsname, &vslen);
	fs = loadfile(fsname, &fslen);
	const char *vv = vs;
	const char *ff = fs;
	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);
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
  p = glCreateProgram();
  glAttachShader(p,v);
  glAttachShader(p,f);
  glLinkProgram(p);
  GLint linked;
	glGetProgramiv(p, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		printf("Error while linking shader %s %s\n", vsname, fsname);
		lua_pushnil(L);
		return 1;
	}
	lua_pushinteger(L, p);
	return 1;
}

static int Lua_Shader_use(lua_State *L)
{
	glUseProgram(luaL_checkint(L, 1));
}

static const struct luaL_Reg shaderlib [] = {
	{"newShader", Lua_Shader_load},
	{"useShader", Lua_Shader_use},
	{NULL, NULL}
};


int luaopen_shader(lua_State *L, const char *parent) {
	luaL_register(L, parent, shaderlib);
	return 1;
}