all:
	g++ -ofast -O3 -std=c++14 -o game -Wall -Wextra -pedantic -g main.cpp sdl_wrapper.cpp digger.cpp world.cpp hud.cpp block.cpp -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
