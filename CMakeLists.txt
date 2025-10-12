cmake_minimum_required(VERSION 4.0)
project(raylib-wasm-template)

set(CMAKE_EXECUTABLE_SUFFIX ".js")
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(SUPPORT_FILEFORMAT_SVG)

# Add compiler flags to handle implicit function declarations (treat as warning, not error)
add_compile_options(-Wno-error=implicit-function-declaration)

# Source files
set(SOURCES
    src/main.cpp
    src/player.cpp
    src/map.cpp
    src/gun.cpp
    src/ui.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s ERROR_ON_UNDEFINED_SYMBOLS=0 -sALLOW_MEMORY_GROWTH -sEXPORTED_RUNTIME_METHODS=printErr --bind --memory")

# Include directories
include_directories(lib/raylib/src)
include_directories(include)

# Configure and add raylib for web platform
if(NOT DEFINED PLATFORM)
    set(PLATFORM "Web" CACHE STRING "")
endif()
add_subdirectory(lib/raylib EXCLUDE_FROM_ALL)

# Link libraries
target_link_libraries(${PROJECT_NAME} raylib)