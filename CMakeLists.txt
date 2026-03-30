cmake_minimum_required(VERSION 3.15)

# Project name, version and language 
project(MiniArc VERSION 0.0.1 LANGUAGES C CXX)

# needed for converting sfx into wave files
include(sonar.cmake)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)

# Set compiler standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# create compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(sdlc)

# add target executable
add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image sdlc)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/inc)

# audio format for sfx
set(WAVE_BITS 16)
set(WAVE_SAMPLE_FREQ 22050)

target_include_directories(sdlc PUBLIC ${CMAKE_SOURCE_DIR}/inc)

target_compile_definitions(sdlc PUBLIC
    SCENE_CLASS=MiniArcGame
    SCENE_HEADER="MiniArc.hpp"
    WINDOW_TITLE="Mini Arcade"
    WINDOW_WIDTH=560
    WINDOW_HEIGHT=896
    RENDER_LOGICAL_WIDTH=140
    RENDER_LOGICAL_HEIGHT=224
    RENDER_VSYNC=1
    TTF_INIT=1
    AUDIO_FORMAT=SDL_AUDIO_S16
    AUDIO_FREQ=${WAVE_SAMPLE_FREQ}
    AUDIO_CHANNELS=1
    AUDIO_NUM_STREAMS=4
)

# Add source files for MiniArc target
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MiniArc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlayerShip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/BackgroundStars.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Enemies.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UILayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HealthBar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GoodyItem.cpp
)

# generate sound effect wave files from xml sources
sonar_generate_wave(alien-shot)
sonar_generate_wave(charged-shot)
sonar_generate_wave(charging)
sonar_generate_wave(explosion)
sonar_generate_wave(game-over)
sonar_generate_wave(hit-effect)
sonar_generate_wave(laser-shot)
sonar_generate_wave(pickup)
sonar_generate_wave(spawn-effect)