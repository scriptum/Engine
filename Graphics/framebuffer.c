#include "../Main.h"
#include "../Macros.h"
#include "SDL_opengl.h"
#include "../render.h"
#include "Graphics.h"

#define checkimage(L) \
	(Lua_Image *)luaL_checkudata(L, 1, "scrupp.image")
	
	
int Lua_Framebuffer_new(lua_State *L)
{
	Lua_Image *ptr;
	unsigned int width, height;
	GLint current_fbo;
	width = luaL_checkinteger(L, 1);
	height = luaL_checkinteger(L, 2);
	if(!supported.FBO) return 0;
	
	//save current fbo
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING_EXT, &current_fbo);
	
	ptr = lua_newuserdata(L, sizeof(Lua_Image));
	ptr->w = width;
	ptr->h = height;
		
	// generate texture save target
	glGenTextures(1, &ptr->texture);
	glBindTexture(GL_TEXTURE_2D, ptr->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// create framebuffer
	glGenFramebuffers_(1, &ptr->fbo);
	glBindFramebuffer_(GL_FRAMEBUFFER_EXT, ptr->fbo);
	glFramebufferTexture2D_(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, ptr->texture, 0);
	//~ glFramebufferRenderbuffer_(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			//~ GL_RENDERBUFFER_EXT, depthbuffer);
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	// unbind framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)current_fbo);
	
	luaL_getmetatable(L, "scrupp.image");
	lua_setmetatable(L, -2);
	return 1;
}

int Lua_Framebuffer_setRenderTarget(lua_State *L)
{
	if(!supported.FBO) return 0;
	if (lua_gettop(L) == 0) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	else
	{
		Lua_Image *image = checkimage(L);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);
	}
	return 0;
}