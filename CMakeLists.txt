cmake_minimum_required(VERSION 4.0)
# use ninja -C build  from parent directory to compile the project
project(Pac-Man LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SDL3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/SDL_Lib/include")
set(SDL3_LIB_DIR "${CMAKE_SOURCE_DIR}/src/SDL_Lib/lib")
set(SDL3_TTF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/TTF_Lib/include")
set(SDL3_TTF_LIB_DIR "${CMAKE_SOURCE_DIR}/src/TTF_Lib/lib")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SDL3_IMAGE_LIB_DIR "${CMAKE_SOURCE_DIR}/src/SDL_Image/lib/x64")
else()
    set(SDL3_IMAGE_LIB_DIR "${CMAKE_SOURCE_DIR}/src/SDL_Image/lib/x86")
endif()
set(SDL3_IMAGE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/SDL_Image/include")

find_library(SDL3_LIBRARY NAMES SDL3 libSDL3 PATHS "${SDL3_LIB_DIR}" NO_DEFAULT_PATH)
find_library(SDL3_TTF_LIBRARY NAMES SDL3_ttf libSDL3_ttf PATHS "${SDL3_TTF_LIB_DIR}" NO_DEFAULT_PATH)
find_library(SDL3_IMAGE_LIBRARY NAMES SDL3_image libSDL3_image PATHS "${SDL3_IMAGE_LIB_DIR}" NO_DEFAULT_PATH)

if(NOT SDL3_LIBRARY)
    message(FATAL_ERROR "SDL3 library not found in ${SDL3_LIB_DIR}")
endif()
if(NOT SDL3_TTF_LIBRARY)
    message(FATAL_ERROR "SDL3_ttf library not found in ${SDL3_TTF_LIB_DIR}")
endif()
if(NOT SDL3_IMAGE_LIBRARY)
    message(FATAL_ERROR "SDL3_image library not found in ${SDL3_IMAGE_LIB_DIR}")
endif()

add_executable(Pac-Man
    Main.cpp
    TileSystem.cpp
    UpdateInput.cpp
    GUI/Windows/MainMenuWindow.cpp
    GUI/Windows/GameWindow.cpp
    GUI/Windows/ErrorWindow.cpp
    GUI/Elements/GUIElement.cpp
    GUI/Windows/GUI.h
    Entities/AI/AIUtils.cpp
    Entities/Entity.cpp
    Entities/Player.cpp
    Entities/Enemy.cpp
    Entities/Pathfinding/Pathfinding.cpp
    Entities/EntityManager.cpp
    Entities/EntityFactory.h

)

target_include_directories(Pac-Man PRIVATE
    "${SDL3_INCLUDE_DIR}"
    "${SDL3_TTF_INCLUDE_DIR}"
    "${SDL3_IMAGE_INCLUDE_DIR}"
)

target_link_libraries(Pac-Man PRIVATE
    "${SDL3_LIBRARY}"
    "${SDL3_TTF_LIBRARY}"
    "${SDL3_IMAGE_LIBRARY}"
)

if(MSVC)
    target_compile_options(Pac-Man PRIVATE /W4 /permissive-)
else()
    target_compile_options(Pac-Man PRIVATE -Wall -Wextra -Wpedantic)
endif()