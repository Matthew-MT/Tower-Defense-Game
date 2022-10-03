CFLAGS:=`sdl2-config --cflags`
LIBS:=`sdl2-config --libs` -lSDL2_ttf

MAIN:=main
EXT:=
SOURCES:=$(MAIN).cpp $(wildcard *.hpp) $(wildcard include/*.hpp)

ifeq ($(OS),Windows_NT)
	EXT=.exe
endif

$(MAIN)$(EXT): $(SOURCES)
	g++ $< -o $@ $(CFLAGS) $(LIBS)

debug: $(SOURCES)
	g++ -g $< -o $(MAIN)_debug$(EXT) $(CFLAGS) $(LIBS) && gdb $(MAIN)_debug$(EXT) && rm $(MAIN)_debug$(EXT)

run:
	./$(MAIN)$(EXT)
