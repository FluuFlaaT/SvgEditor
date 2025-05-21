# 金山训练营结课作业 —— SvgEditor

## 0. 怎么让它跑起来

- 依赖需求
    - vcpkg —— 需要安装并配置$VCPKG_ROOT
    - `qt5-base` `gTest` `tinyxml2` `qt5-Svg` —— 均可透过 vcpkg 安装 
    - CMake，Visual Studio（需支持C++20标准，故推荐v143版及更高版本的构建工具）
    - 未对 macOS 和 Linux 做适配，本项目只保证 Windows 下的可运行性
- 编译
    - `cmake -B build .; cd build; cmake ..; cmake --build --config debug`
        - 两个编译产物：主程序和gtest测试程序
    - `cmake -B build .; cd build; cmake ..; cmake --build --config release`
        - 一个编译产物：主程序