NAME=glapp
LIBS=-lGL -lGLEW -lSDL2 -lfreetype

CXX_OPTS=-std=c++17 -I include -o out/$(NAME)
DEBUG_OPTS=-ggdb

FILES=$(wildcard src/*.cpp) $(wildcard src/sorts/*.cpp)

.PHONY: mkdirs build run clean embuild embuild_asmjs

mkdirs:
	@mkdir -p out/web

build: mkdirs
	@g++ $(CXX_OPTS) $(FILES) $(LIBS)

build_debug: mkdirs
	@g++ $(CXX_OPTS) $(DEBUG_OPTS) $(FILES) $(LIBS)

run: build
	@out/$(NAME)

clean:
	@rm -r out/

.DEFAULT_GOAL := build
