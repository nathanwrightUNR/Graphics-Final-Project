cmake_minimum_required(VERSION 3.10)
project(Wright-PA5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# includes
include_directories(
  /opt/homebrew/opt/glfw/include
  /opt/homebrew/opt/glm/include
  /opt/homebrew/opt/glew/include
  /opt/homebrew/opt/assimp/include
  /Users/nathanwright/Downloads/SOIL2-master/src
)

# src files
set(SOURCES
  main.cpp
  engine.cpp
  camera.cpp
  graphics.cpp
  object.cpp
  shader.cpp
  window.cpp
  mesh.cpp
  sphere.cpp
  texture.cpp
)

add_executable(Wright-PA5 ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

add_library(assimp::assimp SHARED IMPORTED)
set_target_properties(assimp::assimp PROPERTIES
  IMPORTED_LOCATION "/opt/homebrew/Cellar/assimp/5.4.3/lib/libassimp.dylib"
  INTERFACE_INCLUDE_DIRECTORIES "/opt/homebrew/Cellar/assimp/5.4.3/include"
)

# libraries
target_link_libraries(Wright-PA5
  GLEW::glew
  OpenGL::GL
  glfw
  assimp::assimp
  /Users/nathanwright/Downloads/SOIL2-master/build/libSOIL2.a
  "-framework CoreFoundation"
)
