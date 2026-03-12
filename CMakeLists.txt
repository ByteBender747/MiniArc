cmake_minimum_required(VERSION 3.15)

# Project name, version and language 
project(MiniArc VERSION 0.0.1 LANGUAGES C CXX)

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)

# Set compiler standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(sdlc)

# add target executable
add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image sdlc)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/inc)

target_compile_definitions(sdlc PUBLIC
    APP_INIT=MiniArc_Init
    APP_EXIT=MiniArc_Exit
    WINDOW_TITLE="Mini Arcade"
    WINDOW_WIDTH=560
    WINDOW_HEIGHT=896
    RENDER_LOGICAL_WIDTH=140
    RENDER_LOGICAL_HEIGHT=224
)

# Add source files for MiniArc target
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/MiniArc.cpp
)   