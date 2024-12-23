cmake_minimum_required(VERSION 3.21)

project(program)

#for cross-platform std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH  "C:/Qt/6.8.0/msvc2022_64/lib/cmake")
set(CMAKE_CXX_COMPILER "cl")
set(CMAKE_AUTOMOC ON)

# find_library("/lib/tag.lib" tag)
find_library("C:/pkg_static/lib/tag.lib" tag)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)


file(GLOB_RECURSE source_files CONFIGURE_DEPENDS src/*.cpp)
add_executable(program ${source_files})

#target_include_directories(
#    program
#    # INTERFACE ${CMAKE_SOURCE_DIR}/include/mpv
#)

target_link_libraries(
    program
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
    tag
    ${CMAKE_SOURCE_DIR}/lib/libmpv.dll.a
)

