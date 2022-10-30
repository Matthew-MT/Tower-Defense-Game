IMGUI:=imgui/imgui*.cpp
CFLAGS:=`sdl2-config --cflags`
LIBS:=`sdl2-config --libs` -lSDL2_ttf -lSDL2_mixer -lSDL2_image

MAIN:=main
EXT:=
SOURCES:=$(MAIN).cpp $(wildcard *.hpp) $(wildcard include/*.hpp)

ifeq ($(OS),Windows_NT)
	EXT=.exe
else
	EXT=.out
endif

$(MAIN)$(EXT): $(SOURCES)
	g++ $< -o $@ $(IMGUI) $(CFLAGS) $(LIBS)

debug: $(SOURCES)
	g++ -g $< -o $(MAIN)_debug$(EXT) $(IMGUI) $(CFLAGS) $(LIBS) && gdb $(MAIN)_debug$(EXT) && rm $(MAIN)_debug$(EXT)

run:
	./$(MAIN)$(EXT)
