cmake_minimum_required(VERSION 3.14)

# 查找GTest包
find_package(GTest CONFIG REQUIRED)

# 添加Core SVG引擎测试
add_subdirectory(CoreSvgEngine)
