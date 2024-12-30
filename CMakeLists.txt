cmake_minimum_required(VERSION 3.21)

project(Overture)

#TODO: apparently taglib installed from vcpkg can be used but only if cmake --build . --config Release is used during building
# additionally qt can be installed via vcpkg for windows
# later on the goal is to have the build process be vcpkg install zlib utfcpp cppunit taglib qt6

#for cross-platform std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "cl")
set(CMAKE_AUTOMOC ON)
# set(CMAKE_TOOLCHAIN_FILE, "C:/dev_tools/vcpkg/scripts/buildsystems/vcpkg.cmake")
list(APPEND CMAKE_PREFIX_PATH "C:/dev_tools/taglib_installed")
list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.8.0/msvc2022_64")
set(ZLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps")
set(ZLIB_LIBRARY "${CMAKE_SOURCE_DIR}/lib/zlib.lib")



find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(ZLIB REQUIRED)
find_package(TagLib 2.0.0 REQUIRED)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS src/*.cpp)
add_executable(Overture ${source_files})


# link_directories(${CMAKE_SOURCE_DIR}/lib)
# add_library(mpv STATIC IMPORTED)
# set_target_properties(mpv PROPERTIES IMPORT ${CMAKE_SOURCE_DIR}/lib/libmpv.dll.a)

target_link_libraries(
    Overture
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
    ${CMAKE_SOURCE_DIR}/lib/libmpv.dll.a
    ${CMAKE_SOURCE_DIR}/lib/sqlite3.lib
    PRIVATE TagLib::tag 
)

target_compile_definitions(Overture PRIVATE TAGLIB_STATIC)
