SHELL=C:\Users\User\Software\Utility\MSYS\usr\bin\sh

all:
	echo Flags: `sdl2-config --cflags --libs`

main.exe: main.cpp $(wildcard *.hpp)
	g++ $< -o $? -IC:/Users/User/Software/Utility/MSYS/mingw64/include/SDL2 -Dmain=SDL_main -LC:/Users/User/Software/Utility/MSYS/mingw64/lib -lmingw32 -mwindows -lSDL2main -lSDL2
