/*
** $Id: Main.c 50 2009-04-16 12:13:38Z basique $
** Main file
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"

#include "Macros.h"
#include "FileIO.h"
#include "Font.h"
#include "Graphics/Graphics.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Sound.h"
#include "render.h"
#include "SOIL/SOIL.h"
/* cairo binding lua-oocairo */
//#include "lua-oocairo/oocairo.h"



#include <string.h>
#include "SDL_opengl.h"

int done = 0;
float FPS = 60;


/* minimum delta between frames in milliseconds */
int minimumDelta = 10;

/* checks whether an error generated by Lua is actual generated by scrupp.exit() */
/* this lets us exit the app from within Lua */
int check_for_exit(lua_State *L) {
	const char *err_msg;

	err_msg = lua_tostring(L, -1);
	if (strcmp(err_msg, "scrupp.exit") == 0) {
		done = 1;
//		fprintf(stdout, "Exiting Scrupp.\n");
		lua_close(L);
		exit(0);
		return 1;
	} else {
		return 0;
	}
}
/*
static void usage(const char* exec_name) {
	fprintf(stdout, "usage:\n\t%s [Lua file] [arguments]\nor\n\t%s [archive] [arguments]\nor\n\t%s [directory] [arguments]", exec_name, exec_name, exec_name);
	exit(1);
}
*/
int error_function(lua_State *L) {
	const char *err_msg;
	char *pos;
	size_t len;

	err_msg = lua_tolstring(L, -1, &len); /* get error message */
	pos = strstr(err_msg, "scrupp.exit");
	if (pos && (len-(pos-err_msg) == 11)) { /* ends the error message with "scrupp.exit"? */
		lua_pushliteral(L, "scrupp.exit");
		return 1;
	}

	luaL_gsub(L, err_msg, ": ", ":\n\t");
	lua_pushliteral(L, "\n");
	lua_concat(L, 2);

	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_pushvalue(L, -3); /* push error message */

	lua_pushinteger(L, 2); /* start at level 2 */
	lua_call(L, 2, 1);

	return 1;
}

/* Lua functions */

static int Lua_Main_setDelta(lua_State *L) {
	minimumDelta = luaL_checkint(L, 1);
	return 0;
}

static int Lua_Main_fps(lua_State *L) {
	lua_pushinteger(L, (int)FPS);
	return 1;
}

static int Lua_Main_exit(lua_State *L) {
	/* scrupp.exit is implemented by throwing an error with a special message */
	/* the error handler checks for this message using check_for_exit() */
	return luaL_error(L, "scrupp.exit");
}

static int Lua_Main_poll(lua_State *L) {
    SDL_Event event;
    //~ myEventData * data;
    GLuint tex_id;
    //В общем, тут идет дохрена ненужных нам событий (штук 300), которые могут тормозить игру, их нужно пропускать.
    while ( SDL_PollEvent( &event ) ) {
        //printf("|");
        switch ( event.type ) {
        case SDL_QUIT:
            lua_pushstring(L, "q");
            done = 1;
            return 1;

        case SDL_KEYDOWN:
            lua_pushstring(L, "kp");
            lua_pushstring(L, lua_keys[event.key.keysym.sym]);
            lua_pushinteger(L, event.key.keysym.unicode);
            return 3;

        case SDL_KEYUP:
            lua_pushstring(L, "kr");
            lua_pushstring(L, lua_keys[event.key.keysym.sym]);
            lua_pushinteger(L, event.key.keysym.unicode);
            return 3;

        case SDL_MOUSEBUTTONDOWN:
            lua_pushstring(L, "mp");
            lua_pushinteger(L, event.button.x);
            lua_pushinteger(L, event.button.y);
            lua_pushstring(L, buttonNames[event.button.button-1]);
            return 4;

        case SDL_MOUSEBUTTONUP:
            lua_pushstring(L, "mr");
            lua_pushinteger(L, event.button.x);
            lua_pushinteger(L, event.button.y);
            lua_pushstring(L, buttonNames[event.button.button-1]);
            return 4;

        case SDL_VIDEORESIZE:
            lua_pushstring(L, "rz");
            lua_pushinteger(L, event.resize.w);
            lua_pushinteger(L, event.resize.h);
            resize(event.resize.w, event.resize.h);
            return 3;
        //~ case SDL_USEREVENT:
						//~ data = (myEventData *)event.user.data1;
						//~ tex_id = SOIL_internal_create_OGL_texture(
						//~ data->img, data->width, data->height, data->channels,
						//~ 0, SOIL_FLAG_TEXTURE_REPEATS,
						//~ GL_TEXTURE_2D, GL_TEXTURE_2D,
						//~ GL_MAX_TEXTURE_SIZE );
						//~ SOIL_free_image_data( data->img );
						//~ data->ptr->texture = tex_id;
						//~ free(data);
        }
    }
    lua_pushnil(L);
	return 1;
}

static int Lua_Main_clear(lua_State *L) {
	glClear( GL_COLOR_BUFFER_BIT );
	return 0;
}

static int Lua_Main_swap(lua_State *L) {
	SDL_GL_SwapBuffers();
	return 0;
}

static const struct luaL_Reg mainlib [] = {
	{"setDelta", Lua_Main_setDelta},
	{"fps", Lua_Main_fps},
	{"poll", Lua_Main_poll},
	{"clear", Lua_Main_clear},
	{"swapBuffers", Lua_Main_swap},
	{"exit", Lua_Main_exit},
	{NULL, NULL}
};

int luaopen_main(lua_State* L, const char *parent) {
	luaL_register(L, parent, mainlib);	/* leaves table on top of stack */
	lua_pushliteral(L, "VERSION");
	lua_pushliteral(L, VERSION);
	lua_rawset(L, -3);
	lua_pushliteral(L, "PLATFORM");
#ifdef __WIN32__
	lua_pushliteral(L, "Windows");
#elif __MACOSX__
	lua_pushliteral(L, "Mac OS X");
#elif __LINUX__
	lua_pushliteral(L, "Linux");
#else
	lua_pushliteral(L, "Unknown");
#endif
	lua_rawset(L, -3);
	return 1;
}

/* main - function - entry point */
int main(int argc, char *argv[]) {
	lua_State *L;
	Uint32 lastTick;	/* Last iteration's tick value */
	Uint32 delta = 0;
	int i, n, narg;
	char *filename = NULL;

//	fprintf(stdout, "%s v%s\n", PROG_NAME, VERSION);

	if (argc > 1) {
		n = 1;
		if (strcmp(argv[1], "--") != 0)
			filename = argv[1];
	} else {
		n = argc - 1;
	}

	thread_init();

	L = luaL_newstate();	/* initialize Lua */
	luaL_openlibs(L);	/* load Lua base libraries */

	FS_Init(L, argv, &filename);	/* initialize virtual filesystem */

	/* register Lua functions */
	lua_newtable(L);
	luaopen_main(L, NULL);
	luaopen_fileio(L, NULL);
	luaopen_font(L, NULL);
	luaopen_graphics(L, NULL);
	luaopen_sound(L, NULL);
	luaopen_mouse(L, NULL);
	luaopen_keyboard(L, NULL);
	luaopen_thread(L, NULL);
	luaopen_shader(L, NULL);
	//luaopen_movie(L, NULL);
	lua_setglobal(L, NAMESPACE);



	/* put luaopen_oocairo in package.preload["oocairo"] */
	//lua_getglobal(L, "package");
	//lua_getfield(L, -1, "preload");
	//lua_pushcfunction(L, luaopen_oocairo);
	//lua_setfield(L, -2, "oocairo");

	/* put luaopen_socket_core in package.preload["socket.core"] */
	//lua_pushcfunction(L, luaopen_socket_core);
	//lua_setfield(L, -2, "socket.core");

	/* put luaopen_mime_core in package.preload["mime.core"] */
	//lua_pushcfunction(L, luaopen_mime_core);
	//lua_setfield(L, -2, "mime.core");

	//lua_pop(L, 2);

	/* push the error function for the protected calls later on */
	lua_pushcfunction(L, error_function);

	/* load and compile main lua file */
	if (FS_loadFile(L, filename) == FILEIO_ERROR) {
		error(L, "Error loading '%s':\n\t%s", filename, lua_tostring(L, -1));
	}

	/* push all script arguments */
	/* create arg-table for the command-line-arguments */
	/* copied from lua.c of the Lua distribution */
	narg = argc - (n+1); /* number of arguments to the script */
	luaL_checkstack(L, narg+2, "too many arguments to script");
	for (i=n+1; i<argc; i++) {
		lua_pushstring(L, argv[i]);
	}
	lua_createtable(L, narg, n+1);
	for (i=0; i<argc; i++) {
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i - n);
	}
	lua_setglobal(L, "arg");

	/* run the compiled chunk */
	if ((lua_pcall(L, narg, 0, -(narg+2)) != 0) && !check_for_exit(L)) {
		error(L, "Error running '%s':\n\t%s", filename, lua_tostring(L, -1));
	}

	/* the error function stays on the stack */

	if (SDL_GetVideoSurface() == NULL)
		error(L, "Error: " PROG_NAME " was not initialized by " NAMESPACE ".init()!\n");

	
	//enable UNICODE support
	SDL_EnableUNICODE(1);
	/* main loop */
	/* Wait until SDL_QUIT event type (window closed) or a call to scrupp.exit() occurs */
	while ( !done ) {
		lastTick = SDL_GetTicks();

		//glClear( GL_COLOR_BUFFER_BIT );
		glLoadIdentity();

		/* maybe the 'main' table has changed
		   so get a new reference in every cycle */
		lua_getglobal(L, "main");

		if (lua_isnil(L, -1)) {
			error(L, "Error: Table 'main' not found!\n");
		}

		/* main.render(delta) */
		lua_getfield(L, -1, "render");
		lua_pushinteger(L, delta);
		if ((lua_pcall(L, 1, 0, -4) != 0) && !check_for_exit(L)) {
			error(L, "Error running main.render:\n\t%s\n", lua_tostring(L, -1));
		}

		SDL_GL_SwapBuffers();

		lua_pop(L, 1);	/* pop 'main' table */
		delta = SDL_GetTicks() - lastTick;
		if (delta < minimumDelta) {
			SDL_Delay(minimumDelta - delta);
			delta = SDL_GetTicks() - lastTick;
		}
		//SDL_Delay(1);
		FPS = (FPS + 1) / (1 + (float)delta / 1000.0f);
	}
	lua_close(L);
	return 0;
}
