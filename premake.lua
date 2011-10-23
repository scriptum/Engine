package.name = "engine"
package.kind = "exe"
package.language = "c"
package.files = { 
	matchfiles(
		"*.h", "*.c", 
		"SOIL/*.h", 
		"SOIL/*.c", 
		"Graphics/*.h", 
		"Graphics/*.c")
}
project.configs = {"Release"}
project.config["Release"].bindir = "bin/Release"
package.buildflags = { "optimize-size", "no-frame-pointer" }
package.links = {"SDL", "SDLmain", "GL", "lua5.1", "SDL_mixer", "physfs", "IL"}
package.libpaths = { "./lib" }
package.includepaths = {"./include"}
--package.buildoptions = {"`sdl-config --cflags`"}
--package.linkoptions = { "`sdl-config --libs`" }
package.postbuildcommands = {
	"strip -s bin/Release/engine",
	"cp bin/Release/engine /home/rpg/Игры/Monopoly/bin/linux"
	--~ "cd /home/rpg/Игры/Monopoly; ./run_russian.sh"
}