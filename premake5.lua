workspace "MarchingCubes"
  toolset "clang"
  configurations { "Debug", "Release" }
  location "build"

project "MarchingCubes"
  kind "ConsoleApp"
  language "C++"
  targetdir "build"
  targetname "marching_cubes"
  includedirs { "include" }
  links { "GL", "GLEW", "SDL2", "freetype" }
  flags { "FatalWarnings" }

  files { "include/**.h", "src/**.cpp" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

  filter "configurations:Release"
    defines { "RELEASE" }
    optimize "Speed"

  filter {}
  configuration { "linux", "gmake" }
    buildoptions "-std=c++17"

