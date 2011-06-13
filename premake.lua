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
	--"strip -s bin/Release/" .. package.name, 
	"cp bin/Release/" .. package.name .. " /home/rpg/Игры/Monopoly/bin"
	}