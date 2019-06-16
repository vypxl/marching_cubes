NAME=glapp
LIBS=-lGL -lGLEW -lSDL -lfreetype

CXX_OPTS=-I include -o out/$(NAME)

FILES=$(wildcard src/*.cpp) $(wildcard src/sorts/*.cpp)

.PHONY: mkdirs build run clean embuild embuild_asmjs

mkdirs:
	@mkdir -p out/web

build: mkdirs
	@g++ -I include -o out/$(NAME) $(FILES) $(LIBS)

run: build
	@out/$(NAME)

clean:
	@rm -r out/

.DEFAULT_GOAL := build
