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
package.links = {"GL", "lua", "SDL_mixer", "physfs"}
package.buildoptions = { "`sdl-config --cflags` -s -march=prescott" }
package.linkoptions = { "`sdl-config --libs` -s" }
package.postbuildcommands = { 
	"cp /home/rpg/engine/engine/bin/Release/engine /home/rpg/Игры/Monopoly/bin/linux",
	"cd /home/rpg/Игры/Monopoly; ./run_russian.sh"
	}