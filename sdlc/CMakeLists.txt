cmake_minimum_required(VERSION 3.15)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)

# Set compiler standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(sdlc)
target_include_directories(sdlc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_sources(sdlc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PathHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AppEntry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FontRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SpriteRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnimatedSprite.cpp
)