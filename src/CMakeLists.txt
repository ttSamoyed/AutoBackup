# 查找所有源文件
file(GLOB_RECURSE SOURCES "*.cpp")

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(AutoBackup ${SOURCES})
