# SvgEditor

## What is SvgEditor

- A simple SVG painter application build with Qt5
- Easy to build and maintain with CMake
- Fast and tiny
- Modules can be separately used by third-party softwares
- A project made for C++ Summer Intern

## Compile

### Before you compile

- Make sure you installed vcpkg and set `$VCPKG_ROOT`
- Install qt5-base, qt5-Svg, tinyxml2 packages in vcpkg
- Only compile and run it on Windows platform

### Compile command

```bash
mkdir -p build; cd build; cmake .. ; cmake --build . --config Debug
```

## Debug the project

- Requirements
    - Visual Studio 2022
    - Windows 10 +
- Steps
    - Open the `*.sln` in `build/` folder
    - Set `SvgEditor` as Startup project
    - Go `debug` button
    - Start debugging the project