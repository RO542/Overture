cmake_minimum_required(VERSION 3.21)

project(Overture)

#for cross-platform std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "cl")
set(CMAKE_AUTOMOC ON)
set(CMAKE_TOOLCHAIN_FILE, "C:/dev_tools/vcpkg/scripts/buildsystems/vcpkg.cmake")
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
# target_include_directories(program PRIVATE "C:/dev_tools/vcpkg/packages/taglib_x64-windows/include/taglib")

target_link_libraries(
    Overture
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui
    ${CMAKE_SOURCE_DIR}/lib/libmpv.dll.a
    PRIVATE TagLib::tag 
)
target_compile_definitions(Overture PRIVATE TAGLIB_STATIC)
target_compile_definitions(Overture PRIVATE ZLIB_STATIC)