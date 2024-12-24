
<# Release 
$env:VCPKG_ROOT="C:/dev_tools/vcpkg"
$env:TAGLIB_SRC_DIR="$HOME/desktop/taglib-2.0.2/taglib-2.0.2"
$env:TAGLIB_DST_DIR="C:/dev_tools/taglib_built"
cd $env:TAGLIB_SRC_DIR

cmake -B $env:TAGLIB_DST_DIR -DBUILD_SHARED_LIBS=OFF -DVISIBILITY_HIDDEN=ON -DBUILD_TESTING=ON `
  -DCMAKE_POLICY_DEFAULT_CMP0074=NEW `
  -DENABLE_STATIC_RUNTIME=ON `
  -DZLIB_ROOT="C:/dev_tools/vcpkg/installed/x64-windows" `
  -DZLIB_INCLUDE_DIR="C:/dev_tools/vcpkg/installed/x64-windows/include" `
  -DZLIB_LIBRARY="C:/dev_tools/vcpkg/installed/x64-windows/lib" `
  -DBUILD_EXAMPLES=ON `
  -DBUILD_BINDINGS=OFF `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_TOOLCHAIN_FILE="C:/dev_tools/vcpkg/scripts/buildsystems/vcpkg.cmake" `
  -G "Visual Studio 17 2022"

cmake --build $env:TAGLIB_DST_DIR --config Release
cmake --install $env:TAGLIB_DST_DIR --config Release --prefix C:/dev_tools/taglib_installed
#>


#Debug Mode enable_static_runtime=off
$env:VCPKG_ROOT="C:/dev_tools/vcpkg"
$env:TAGLIB_SRC_DIR="$HOME/desktop/taglib-2.0.2/taglib-2.0.2"
$env:TAGLIB_DST_DIR="C:/dev_tools/taglib_built"
cd $env:TAGLIB_SRC_DIR

cmake -B $env:TAGLIB_DST_DIR -DBUILD_SHARED_LIBS=OFF -DVISIBILITY_HIDDEN=ON -DBUILD_TESTING=ON `
  -DCMAKE_POLICY_DEFAULT_CMP0074=NEW `
  -DENABLE_STATIC_RUNTIME=OFF `
  -Dutf8cpp_INCLUDE_DIR="C:/dev_tools/vcpkg/installed/x64-windows/include" `
  -DZLIB_ROOT="C:/dev_tools/vcpkg/installed/x64-windows" `
  -DZLIB_INCLUDE_DIR="C:/dev_tools/vcpkg/installed/x64-windows/include" `
  -DZLIB_LIBRARY="C:/dev_tools/vcpkg/installed/x64-windows/lib" `
  -DBUILD_EXAMPLES=ON `
  -DBUILD_BINDINGS=OFF `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_TOOLCHAIN_FILE="C:/dev_tools/vcpkg/scripts/buildsystems/vcpkg.cmake" `
  -G "Visual Studio 17 2022"

cmake --build $env:TAGLIB_DST_DIR --config Debug 
cmake --install $env:TAGLIB_DST_DIR --config Debug --prefix C:/dev_tools/taglib_installed