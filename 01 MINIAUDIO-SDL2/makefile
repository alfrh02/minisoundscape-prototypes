local:
	g++ src/main.cpp `sdl2-config --cflags --libs` -o bin/out

web:
	emcc src/main.cpp `sdl2-config --cflags --libs` -sUSE_SDL=2 -o bin/index.html

run:
	./bin/out

serve:
	emrun ./bin/index.html
