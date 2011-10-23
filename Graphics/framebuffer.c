#include "../Main.h"
#include "../Macros.h"
#include "SDL_opengl.h"
#include "../render.h"
#include "Graphics.h"

#define checkimage(L) \
	(Lua_Image *)luaL_checkudata(L, 1, "scrupp.image")
	
int checkFramebufferStatus() {
    GLenum status;
    status = (GLenum) glCheckFramebufferStatus_(GL_FRAMEBUFFER_EXT);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            return 1;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
	    printf("Framebuffer incomplete, incomplete attachment\n");
            return 0;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	    printf("Unsupported framebuffer format\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
	    printf("Framebuffer incomplete, missing attachment\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
	    printf("Framebuffer incomplete, attached images must have same dimensions\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
	    printf("Framebuffer incomplete, attached images must have same format\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
	    printf("Framebuffer incomplete, missing draw buffer\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
	    printf("Framebuffer incomplete, missing read buffer\n");
            return 0;
    }
    return 0;
}
int Lua_Framebuffer_new(lua_State *L)
{
	Lua_Image *ptr;
	unsigned int width, height, percision, alpha;
	GLint current_fbo;
	GLenum internal, format;
	
	if(!supported.FBO) return 0;
	
	width = luaL_checkinteger(L, 1);
	height = luaL_checkinteger(L, 2);
	percision = lua_tointeger(L, 3);
	alpha = lua_toboolean(L, 4);
	
	if(percision == 32)
	{
		internal = alpha ? GL_RGBA32F_ARB : GL_RGB32F_ARB;
		format = GL_FLOAT;
	}
	else if(percision == 16)
	{
		internal = alpha ? GL_RGBA16F_ARB : GL_RGB16F_ARB;
		format = GL_FLOAT;
	}
	else
	{
		internal = alpha ? GL_RGBA : GL_RGB;
		format = GL_UNSIGNED_BYTE;
	}
	
	//save current fbo
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING_EXT, &current_fbo);
	
	ptr = lua_newuserdata(L, sizeof(Lua_Image));
	ptr->w = width;
	ptr->h = height;
		
	// generate texture save target
	glGenTextures(1, &ptr->texture);
	glBindTexture(GL_TEXTURE_2D, ptr->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//~ glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D,0,internal,width,height,0,GL_RGBA,format,0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// create framebuffer
	glGenFramebuffers_(1, &ptr->fbo);
	glBindFramebuffer_(GL_FRAMEBUFFER_EXT, ptr->fbo);
	glFramebufferTexture2D_(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, ptr->texture, 0);
	checkFramebufferStatus();

	// unbind framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)current_fbo);
	
	luaL_getmetatable(L, "scrupp.image");
	lua_setmetatable(L, -2);
	return 1;
}

int Lua_Framebuffer_setRenderTarget(lua_State *L)
{
	if(!supported.FBO) return 0;
	if (lua_gettop(L) == 0) 
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		//~ glDrawBuffer(GL_BACK);
		glViewport( 0, 0, screen->w, screen->h );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( 0, screen->w, screen->h, 0, -1, 1 );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	}
	else
	{
		Lua_Image *image = checkimage(L);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, image->fbo);
		//~ glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glViewport( 0, 0, image->w, image->h );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( 0, image->w, image->h, 0, -1, 1 );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	}
	return 0;
}