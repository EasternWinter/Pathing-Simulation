cmake_minimum_required(VERSION 3.20)

project(PathingSimulation)

# Set the C++ standard. Your code uses C++17 features.
set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#target_include_directories(pathing_sim PRIVATE
#    /opt/homebrew/include
#)
include_directories(/opt/homebrew/include)
# Find the SFML package.
# The components 'graphics', 'window', and 'system' are commonly needed.
# You might need to set SFML_DIR if you installed SFML in a non-standard location.
# Example: cmake -D SFML_DIR=/path/to/sfml/lib/cmake/SFML ..
find_package(SFML REQUIRED COMPONENTS Graphics Window System)
#find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

add_executable(pathing_sim testing.cpp)

# Link the SFML libraries to your executable
target_link_libraries(pathing_sim PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
#target_link_libraries(pathing_sim PRIVATE sfml-graphics)