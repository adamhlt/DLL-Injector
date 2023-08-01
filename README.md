```
                       ____  __    __       ____        _           __            
                      / __ \/ /   / /      /  _/___    (_)__  _____/ /_____  _____
                     / / / / /   / /       / // __ \  / / _ \/ ___/ __/ __ \/ ___/
                    / /_/ / /___/ /___   _/ // / / / / /  __/ /__/ /_/ /_/ / /  
                   /_____/_____/_____/  /___/_/ /_/_/ /\___/\___/\__/\____/_/  
                                                /___/                        


                            DLL Injector (LoadLibrary) in C++ (x86 / x64) 
                                      LoadLibrary DLL Injector
```
<p align="center">
    <img src="https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=for-the-badge&logo=appveyor" alt="C++">
    <img src="https://img.shields.io/badge/platform-Windows-0078d7.svg?style=for-the-badge&logo=appveyor" alt="Windows">
    <img src="https://img.shields.io/badge/arch-x86-red.svg?style=for-the-badge&logo=appveyor" alt="x86">
    <img src="https://img.shields.io/badge/arch-x64-green.svg?style=for-the-badge&logo=appveyor" alt="x64">
</p>

## :open_book: Project Overview :

This is a DLL injector written in C++, it uses the most basic method to inject DLL (LoadLibrary).

The "Release" section contains the DLL injector (x86 / x64) and tiny test programs (x86 / x64).

## 🚀 Getting Started :

### Visual Studio :

1. Open the solution file (.sln).
2. Build the project in Realese (x86) or Release (x64).

Every configuration in x86 / x64 (Debug and Realese) are already configured.

> **Warning** <br>
> It is necessary to build it in x86 or x64, it depends of the target process's architecture.

### Other IDE using CMAKE :

This **CMakeLists.txt** should compile the project.

```cmake
cmake_minimum_required(VERSION 3.0)
project(Basic_DLL_Injector)

set(CMAKE_CXX_STANDARD 17)

add_executable(inject DLL_Injector.cpp)
```

Tested on CLion with MSVC compiler, you can get Visual Studio Build Tools [**here**](https://visualstudio.microsoft.com/fr/downloads/?q=build+tools).

## 🧪 Usage :

### How to use the program :

Use it in the command line :

    inject.exe <dll> <process>
**You can get the DLL injector (x86 / x64) and the test files (x86 / x64) in the "Release" section.**

### Demonstration :

### TEST FILES (x86)

https://user-images.githubusercontent.com/48086737/170119196-8610ee0b-984e-4a42-a931-007dccd3d8c0.mp4
