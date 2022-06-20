cmake_minimum_required(VERSION 3.14)
project(ddpaint C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-mwindows -municode")

include_directories("D:\\msys64\\mingw64\\include\\skia")
include_directories("D:\\msys64\\mingw64\\include\\skia\\include\\core")

add_executable(
    mashiro
    src/main.cc
)

target_link_libraries(
    a
    skia
    libjpeg
    libpng
    libwebp
    libwebpdecoder
    libwebpdemux
    libwebpmux
)
