#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>



#include "../SOIL/SOIL.h"
#include "../Main.h"
#include "../Macros.h"
#include "Graphics.h"
#include "../physfsrwops.h"

#define checkimage(L) \
	(Lua_Image *)luaL_checkudata(L, 1, "scrupp.image")

int load_image_from_string(lua_State *L, const unsigned char *myBuf, unsigned int file_size) {
    Lua_Image *ptr;
    int width, height, channels;
	unsigned int tex_id;
	unsigned char* img;
    img = SOIL_load_image_from_memory(
					myBuf, sizeof(unsigned char) * file_size,
					&width, &height, &channels,
					0 );
	if( NULL == img )
		return 0;
	tex_id = SOIL_internal_create_OGL_texture(
			img, width, height, channels,
			0, SOIL_FLAG_TEXTURE_REPEATS,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE );
	/*	and nuke the image data	*/
	SOIL_free_image_data( img );
	ptr = lua_newuserdata(L, sizeof(Lua_Image));
	ptr->texture = tex_id;
	ptr->w = width;
	ptr->h = height;
	luaL_getmetatable(L, "scrupp.image");
	lua_setmetatable(L, -2);
	return 1;
}

int Lua_Image_load(lua_State *L) {
	const char* filename = luaL_checkstring(L, 1);
	PHYSFS_file* myfile = PHYSFS_openRead(filename);
    if (!myfile)
		return luaL_error(L, "Error loading file '%s': %s", filename, SDL_GetError());
    unsigned int file_size = PHYSFS_fileLength(myfile);
    unsigned char *myBuf;
    myBuf = (unsigned char *)malloc(sizeof(unsigned char) * file_size);
    PHYSFS_read (myfile, myBuf, sizeof(unsigned char), file_size);
    PHYSFS_close(myfile);
    int res = load_image_from_string(L, myBuf, file_size);
    free(myBuf);
    if(!res) return luaL_error(L, "Error loading file '%s': %s", filename, stbi_failure_reason());
	return 1;
}

int Lua_Image_loadFromString(lua_State *L) {
	unsigned int file_size;
	const unsigned char* myBuf = luaL_checklstring(L, 1, &file_size);
	int res = load_image_from_string(L, myBuf, file_size);
	if(!res) return luaL_error(L, "Error loading file from string: %s", stbi_failure_reason());
	return 1;
}


int Lua_Image_getWidth(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->w);
	return 1;
}

int Lua_Image_getHeight(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->h);
	return 1;
}

int Lua_Image_getTextureId(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->texture);
	return 1;
}

int Lua_Image_getSize(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->w);
	lua_pushinteger(L, image->h);
	return 2;
}

#define LUA_GET_TONUM(var) lua_getfield(L, -1, #var);\
	    var = lua_tonumber(L, -1);\
	    lua_pop(L, 1);
#define LUA_GET_CHCKNUM(var) lua_getfield(L, -1, #var);\
	    var = luaL_checknumber(L, -1);\
	    lua_pop(L, 1);

int Lua_Image_draw(lua_State *L) {
	Lua_Image *image = checkimage(L);
	float x,y,angle,w,h,tw,th;
	if(lua_istable(L, 2)) {
	    LUA_GET_CHCKNUM(x)
	    LUA_GET_CHCKNUM(y)
        LUA_GET_TONUM(angle)
	    LUA_GET_TONUM(w)
	    LUA_GET_TONUM(h)
	    LUA_GET_TONUM(tw)
	    LUA_GET_TONUM(th)
	} else {
        x = luaL_checknumber(L, 2);
        y = luaL_checknumber(L, 3);
        angle = lua_tonumber(L, 4);
        w = lua_tonumber(L, 5);
        h = lua_tonumber(L, 6);
        tw = lua_tonumber(L, 7);
        th = lua_tonumber(L, 8);
	}
	if(!w) w = image->w;
	if(!h) h = w * image->h / image->w;
	tw = tw ? w/tw : 1;
	th = th ? h/th : 1;
	/* save the modelview matrix */
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angle,0,0,1);
	//glScaled(sx,sy,0);
	glBindTexture(GL_TEXTURE_2D, image->texture);
    glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);         glVertex2i(0, 0);
    glTexCoord2f(0, th);       glVertex2i(0, h);
    glTexCoord2f(tw, th);     glVertex2i(w, h);
    glTexCoord2f(tw, 0);       glVertex2i(w, 0);
    glEnd();
	glPopMatrix();
    glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	return 0;
}

int Lua_Image_drawq(lua_State *L) {
	Lua_Image *image = checkimage(L);
	float x,y,angle,w,h,qx,qy,qw,qh;
	if(lua_istable(L, 2))
	{
	    LUA_GET_TONUM(x)
	    LUA_GET_TONUM(y)
        LUA_GET_TONUM(angle)
	    LUA_GET_TONUM(w)
	    LUA_GET_TONUM(h)
	    LUA_GET_TONUM(qx)
	    LUA_GET_TONUM(qy)
	    LUA_GET_TONUM(qw)
	    LUA_GET_TONUM(qh)
	}
	else
	{
        x = luaL_checknumber(L, 2);
        y = luaL_checknumber(L, 3);
        angle = lua_tonumber(L, 4);
        w = lua_tonumber(L, 5);
        h = lua_tonumber(L, 6);
        qx = lua_tonumber(L, 7);
        qy = lua_tonumber(L, 8);
        qw = lua_tonumber(L, 9);
        qh = lua_tonumber(L, 10);
	}
	if(!w) w = image->w;
	if(!h) h = w * image->h / image->w;
	qx = qx ? qx/(float)image->w : 0;
	qy = qy ? qy/(float)image->h : 0;
	qw = qw ? qx + qw/(float)image->w : 1;
	qh = qh ? qy + qh/(float)image->h : 1;
	/* save the modelview matrix */
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angle,0,0,1);
	//glScaled(sx,sy,0);
	glBindTexture(GL_TEXTURE_2D, image->texture);
    glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(qx, qy);   glVertex2i(0, 0);
    glTexCoord2f(qx, qh);   glVertex2i(0, h);
    glTexCoord2f(qw, qh);   glVertex2i(w, h);
    glTexCoord2f(qw, qy);   glVertex2i(w, 0);
    glEnd();
	glPopMatrix();
    glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	return 0;
}

int image_gc(lua_State *L) {
	Lua_Image *image = checkimage(L);
	glDeleteTextures(1, &image->texture);
	return 0;
}

int image_tostring(lua_State *L) {
	lua_pushliteral(L, "Image");
	return 1;
}
