build:
	gcc -Wall -std=c99 -I/opt/homebrew/include src/*.c -L/opt/homebrew/lib -lSDL2 -lm -o renderer.exe

run:
	./renderer.exe