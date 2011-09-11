#include <math.h>



#include "../SOIL/SOIL.h"
#include "../Main.h"
#include "../Macros.h"
#include "Graphics.h"
#include "../physfsrwops.h"

#define checkimage(L) \
	(Lua_Image *)luaL_checkudata(L, 1, "scrupp.image")

//~ int image_loader(void * data)
//~ {
  //~ int width, height, channels;
  //~ GLuint tex_id;
  //~ unsigned char* img;
  //~ unsigned int file_size;
  //~ unsigned char *myBuf;
  //~ PHYSFS_file* myfile;
  //~ //while (1) 
  //~ {
    //~ SDL_LockMutex(imgmutex);
    //~ int i;
    //~ for (i = 0; i < IMG_LOAD_SLOTS; i++)
      //~ {
	//~ if(img_load_slot[i])
	//~ {
	  //~ myfile = PHYSFS_openRead(img_load_slot[i]->file);
	  //~ if (!myfile)
	    //~ printf("Error loading file '%s': %s", img_load_slot[i]->file, SDL_GetError());
	  //~ file_size = PHYSFS_fileLength(myfile);
	  //~ 
	  //~ myBuf = (unsigned char *)malloc(sizeof(unsigned char) * file_size);
	  //~ PHYSFS_read (myfile, myBuf, sizeof(unsigned char), file_size);
	  //~ PHYSFS_close(myfile);
	  //~ if( NULL == myBuf )
	    //~ printf("Error loading file '%s': %s", img_load_slot[i]->file, SDL_GetError());
	  //~ printf("%s\n", img_load_slot[i]->file);
	  //~ img = SOIL_load_image_from_memory(
					  //~ myBuf, sizeof(unsigned char) * file_size,
					  //~ &width, &height, &channels,
					  //~ 0 );
	  //~ 
	  //~ printf("%d\n", img);
	  //~ tex_id = SOIL_internal_create_OGL_texture(
			  //~ img, width, height, channels,
			  //~ 0, 0,
			  //~ GL_TEXTURE_2D, GL_TEXTURE_2D,
			  //~ GL_MAX_TEXTURE_SIZE );
	  //~ SOIL_free_image_data( img );
	  //~ 
	  //~ free(myBuf);
	  //~ printf("%s\n", img_load_slot[i]->file);
	  //~ img_load_slot[i]->image->w = width;
	  //~ img_load_slot[i]->image->h = height;
	  //~ img_load_slot[i]->image->texture = tex_id;
	  //~ free(img_load_slot[i]->file);
	  //~ printf("%d\n", img);
	  //~ free(img_load_slot[i]);
	  //~ img_load_slot[i] = NULL;
	  //~ break;
	//~ }
      //~ }
    //~ SDL_UnlockMutex(imgmutex);
    //~ SDL_Delay(10);
  //~ }
//~ }

int load_image_from_string(lua_State *L, const unsigned char *myBuf, unsigned int file_size, int repeat) {
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
			0, SOIL_FLAG_TEXTURE_REPEATS * repeat,
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
    const char* filename;
	int repeat, res;
	PHYSFS_file* myfile;
	unsigned char *myBuf;
	unsigned int file_size;
	filename = luaL_checkstring(L, 1);
    
	repeat = lua_toboolean(L, 2);
    myfile = PHYSFS_openRead(filename);
    if (!myfile)
      return luaL_error(L, "Error loading file '%s': %s", filename, SDL_GetError());
    file_size = PHYSFS_fileLength(myfile);
    
    myBuf = (unsigned char *)malloc(sizeof(unsigned char) * file_size);
    PHYSFS_read (myfile, myBuf, sizeof(unsigned char), file_size);
    PHYSFS_close(myfile);
    res = load_image_from_string(L, myBuf, file_size, repeat);
    free(myBuf);
    if(!res) return luaL_error(L, "Error loading file '%s': %s", filename, stbi_failure_reason());
	return 1;
}

//~ int Lua_Image_load(lua_State *L) {
    //~ const char* filename = luaL_checkstring(L, 1);
    //~ Lua_Image *ptr;
    //~ ptr = lua_newuserdata(L, sizeof(Lua_Image));
    //~ ptr->texture = null_texture;
    //~ ptr->w = 1;
    //~ ptr->h = 1;
    //~ luaL_getmetatable(L, "scrupp.image");
    //~ lua_setmetatable(L, -2);
    //~ //find free slot
    //~ int i;
    //~ SDL_LockMutex(imgmutex);
    //~ for (i = 0; i < IMG_LOAD_SLOTS; i++)
    //~ {
      //~ if(!img_load_slot[i])
      //~ {
	//~ img_load_slot[i] = (img_load_request*)malloc(sizeof(img_load_request));
	//~ img_load_slot[i]->file = (char*)malloc(strlen(filename)+1);
	//~ strcpy(img_load_slot[i]->file, filename);
	//~ printf("send %s\n", img_load_slot[i]->file);
	//~ img_load_slot[i]->image = ptr;
	//~ break;
      //~ }
    //~ }
    //~ SDL_UnlockMutex(imgmutex);
    //~ return 1;
//~ }

int Lua_Image_loadFromString(lua_State *L) {
	unsigned int file_size;
	const unsigned char* myBuf = luaL_checklstring(L, 1, &file_size);
	int repeat = lua_toboolean(L, 2);
	int res = load_image_from_string(L, myBuf, file_size, repeat);
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
	    //LUA_GET_TONUM(tw)
	    //LUA_GET_TONUM(th)
	} else {
        x = luaL_checknumber(L, 2);
        y = luaL_checknumber(L, 3);
        angle = lua_tonumber(L, 4);
        w = lua_tonumber(L, 5);
        h = lua_tonumber(L, 6);
        //tw = lua_tonumber(L, 7);
        //th = lua_tonumber(L, 8);
	}
	if(!w) w = image->w;
	if(!h) h = w * image->h / image->w;
	//tw = tw ? w/tw : 1;
	//th = th ? h/th : 1;
	/* save the modelview matrix */
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angle,0,0,1);
	
	glBindTexture(GL_TEXTURE_2D, image->texture);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glScalef(w, h, 0);
	glCallList(quadlist);
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
	qx = qx/(float)image->w;
	qy = qy/(float)image->h;
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
