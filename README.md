![Banner](https://user-images.githubusercontent.com/48086737/170106580-716a5608-65f4-40c8-bda6-479cf17632e4.png)

# DLL Injector

 [![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=for-the-badge&logo=appveyor)](https://en.wikipedia.org/wiki/C%2B%2B) [![Windows](https://img.shields.io/badge/platform-Windows-0078d7.svg?style=for-the-badge&logo=appveyor)](https://en.wikipedia.org/wiki/Microsoft_Windows) [![x86](https://img.shields.io/badge/arch-x86-red.svg?style=for-the-badge&logo=appveyor)](https://en.wikipedia.org/wiki/X86) [![x64](https://img.shields.io/badge/arch-x64-green.svg?style=for-the-badge&logo=appveyor)](https://en.wikipedia.org/wiki/X64)

## :open_book: Project Overview :

This is a DLL injector written in C++, it uses the most basic method to inject DLL (LoadLibrary).

The "Release" section contains the DLL injector (x86 / x64) and tiny test programs (x86 / x64).

## ๐ Getting Started :

### Visual Studio :

1. Open the solution file (.sln).
2. Build the project in Realese (x86) or Release (x64).

Every configuration in x86 / x64 (Debug and Realese) are already configured.

**It is necessary to build it in x86 or x64, it depends of the target process's architecture.**

### Other IDE using CMAKE :

This **CMakeLists.txt** should compile the project.

```cmake
cmake_minimum_required(VERSION 3.0)
project(Basic_DLL_Injector)

set(CMAKE_CXX_STANDARD 17)

add_executable(inject DLL_Injector.cpp)
```

Tested on CLion with MSVC compiler, you can get Visual Studio Build Tools [**here**](https://visualstudio.microsoft.com/fr/downloads/?q=build+tools).

## ๐งช Usage :

### How to use the program :

Use it in the command line :

    inject.exe <dll> <process>
**You can get the DLL injector (x86 / x64) and the test files (x86 / x64) in the "Release" section.**

### Demonstration :

### TEST FILES (x86)

https://user-images.githubusercontent.com/48086737/170119196-8610ee0b-984e-4a42-a931-007dccd3d8c0.mp4
