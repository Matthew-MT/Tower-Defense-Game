CFLAGS:=`sdl2-config --cflags`
LIBS:=`sdl2-config --libs`

MAIN:=main

$(MAIN): main.cpp $(wildcard *.hpp) $(wildcard include/*.hpp)
	g++ $< -o $@ $(CFLAGS) $(LIBS)

ifeq ($(OS),Windows_NT)
run:
	./$(MAIN).exe
else
run:
	./$(MAIN)
endif
