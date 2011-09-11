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
package.buildflags = { "optimize-speed", "no-frame-pointer" }
package.links = {"GL", "lua5.1", "SDL_mixer", "physfs"}
package.buildoptions = { "`sdl-config --cflags` -I./include -L./lib" }
package.linkoptions = { "`sdl-config --libs` -L./lib" }
--~ package.postbuildcommands = {
	--~ "cp /home/rpg/engine/engine/bin/Release/engine /home/rpg/Игры/Monopoly/bin/linux",
	--~ "cd /home/rpg/Игры/Monopoly; ./run_russian.sh"
--~ }