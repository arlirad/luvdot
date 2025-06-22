cmake_minimum_required(VERSION 3.10)
project(luvdot)

find_package(OpenCV REQUIRED)
find_package(Lua REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${LUA_INCLUDE_DIR})

add_executable(luvdot 
    src/main.cpp
)
target_link_libraries(luvdot ${OpenCV_LIBS} ${LUA_LIBRARIES})