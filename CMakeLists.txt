cmake_minimum_required(VERSION 3.24)

project(SurfaceScry VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(KF6 REQUIRED COMPONENTS WindowSystem CoreAddons)

add_subdirectory(src/libsurfacescry)
add_subdirectory(src/surfacescry-gui)
