all:
	g++ -O0 -std=c++14 -o game -Wall -Wextra -pedantic -g src/*.cpp -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
