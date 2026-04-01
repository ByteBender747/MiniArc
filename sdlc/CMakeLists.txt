cmake_minimum_required(VERSION 3.15)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)

# Set compiler standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_library(sdlc)

target_link_libraries(sdlc PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)
target_include_directories(sdlc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_sources(sdlc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AppLayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AppEntry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FontRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SpriteRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnimatedSprite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ColorConverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Keyboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Painter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioDataBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CSVParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TextInput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ConfigFile.cpp
)