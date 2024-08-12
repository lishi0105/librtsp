# aarch64-linux-gnu.cmake
# 使用 CMake 进行 aarch64 架构的交叉编译的工具链文件

# 指定交叉编译器
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# 指定交叉编译器的位置
# 修改这些路径以匹配你的交叉编译器的实际安装位置
set(COMPILER "aarch64-linux-gnu-")
set(CMAKE_C_COMPILER "/usr/bin/${COMPILER}gcc-9")
set(CMAKE_CXX_COMPILER "/usr/bin/${COMPILER}g++-9")
set(CMAKE_STRIP        /usr/bin/${COMPILER}strip CACHE FILEPATH "")
set(CMAKE_RANLIB       /usr/bin/${COMPILER}ranlib CACHE FILEPATH "")
set(CMAKE_AR           /usr/bin/${COMPILER}ar CACHE FILEPATH "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -mtune=cortex-a72 -mcpu=cortex-a72")

# 交叉编译时，你需要手动指定系统根目录（Sysroot）
# 这是交叉编译环境的根目录，包含库和头文件
# 如果你有一个完整的文件系统，或者使用了如 sysroot 的东西
# 请确保指定它的位置，例如：
set(CMAKE_FIND_ROOT_PATH "/usr/aarch64-linux-gnu/")

# 设置查找程序时只在指定目录下查找
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# 设置只在根目录下查找库和头文件
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# 避免 CMake 在系统路径中查找程序库
# set(CMAKE_SYSROOT "/usr/aarch64-linux-gnu")
