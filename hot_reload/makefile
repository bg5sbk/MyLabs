all: game game_core.so

game: game.c game.h game_core.h
	cc -o game game.c -ldl

game_core.so: game_core.c game_core.h game.h
	cc -fPIC -shared -o game_core.so game_core.c

clean:
	rm game game_core.so
