release:
	g++ src/main.cpp -o voxelize -lSDL2 -lGL -lGLEW

debug:
	g++ src/main.cpp -fsanitize=address -o voxelize -lSDL2 -lGL -lGLEW