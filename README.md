Forget spells from the magic menu.

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2019](https://visualstudio.microsoft.com/)
	* Desktop development with C++

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/Exit-9B/ForgetSpell
cd ForgetSpell
git submodule update --init --recursive

# Skyrim SE
cmake --preset vs2022-windows
cmake --build build --config Release

# Skyrim VR
cmake --preset vs2022-windows-vr
cmake --build buildVR --config Release
```
