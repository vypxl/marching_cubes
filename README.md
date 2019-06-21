# Marching Cubes

This is a demo of the marching cubes algorithm which I created just for fun.

## How to build (Linux)
You will need `premake5`, `make`, `gcc/g++`, `freetype2`, `glew`, `glfw`, `glm` and `opengl` installed.

Build: ```bash
$ premake5 gmake2
$ cd build
$ make config=[debug | release] # choose one
```
Run: `make run` (in build directory)
Delete build files: `make clean`

Built executable can be found in `build/marching_cubes` or run directly via `make run`.

