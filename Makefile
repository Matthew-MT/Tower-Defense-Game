CFLAGS := $(sdl2-config --cflags)
LIBS := $(sdl2-config --libs)

all:
	echo Shell: $(SHELL)
	echo Flags: $(CFLAGS) && echo Libs: $(LIBS)

main.exe: main.cpp $(wildcard *.hpp) $(wildcard include/*.hpp)
	g++ $< -o $? -IC:/Users/User/Software/Utility/MSYS/mingw64/include/SDL2 -Dmain=SDL_main -LC:/Users/User/Software/Utility/MSYS/mingw64/lib -lmingw32 -mwindows -lSDL2main -lSDL2
