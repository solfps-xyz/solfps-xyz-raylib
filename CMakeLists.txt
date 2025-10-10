cmake_minimum_required(VERSION 3.0)
project(raylib-wasm-template)

set(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(SUPPORT_FILEFORMAT_SVG)

add_executable(${PROJECT_NAME} src/main.cpp)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s ERROR_ON_UNDEFINED_SYMBOLS=0 -sALLOW_MEMORY_GROWTH -sEXPORTED_RUNTIME_METHODS=printErr --bind --memory")
set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.

include_directories(lib/raylib/src)
find_library(RL_LIBRARY NAMES raylib PATHS lib/raylib/src NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
target_link_libraries(${PROJECT_NAME} ${RL_LIBRARY})