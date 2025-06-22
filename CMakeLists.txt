cmake_minimum_required(VERSION 3.10)
project(luvdot)

find_package(OpenCV REQUIRED)
find_package(Lua REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${LUA_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(luvdot 
    src/color.cpp
    src/conversion.cpp
    src/il.cpp
    src/img.cpp
    src/main.cpp
)
target_link_libraries(luvdot ${OpenCV_LIBS} ${LUA_LIBRARIES})

set_property(TARGET luvdot PROPERTY CXX_STANDARD 20)