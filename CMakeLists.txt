# CMakeList.txt : CMake project for autologin, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(autologin VERSION 1.0.0 LANGUAGES CXX)
# Add source to this project's executable.
add_executable (autologin "main.cpp")

# TODO: Add tests and install targets if needed.
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(autologin PRIVATE cpr::cpr)

