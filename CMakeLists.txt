# Require a modern CMake version
cmake_minimum_required(VERSION 3.24)

# Name your game
project(Tetris)

# 1. Tell CMake to download SFML from the internet automatically
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.1.0  # Or the specific SFML 3 branch you want
)
FetchContent_MakeAvailable(SFML)

# 2. Build your executable from your source code
add_executable(tetris tetris.cpp)

# 3. Connect the SFML Graphics module to your executable
target_link_libraries(tetris PRIVATE SFML::Graphics)

# Automatically copy the assets folder to the build directory during compilation
add_custom_command(
    TARGET tetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:tetris>/assets"
)