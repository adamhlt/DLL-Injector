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

This is a DLL injector written in C++, it uses the most basic method to inject a DLL (LoadLibrary).

The "Release" section contains the DLL injector (x86 / x64) and tiny test programs (x86 / x64).

## :file_folder: Project Structure :

```
.
├── CMakeLists.txt              CMake build (recommended for other IDEs / CLI)
├── include/injector/           Public headers of the injector library
│   ├── error.hpp               FormatError / FormatLastError
│   ├── injector.hpp            InjectDll
│   └── process.hpp             FindProcessId
├── src/                        Implementation and command-line front end
│   ├── cli.{hpp,cpp}           Argument parsing and usage text
│   ├── error.cpp
│   ├── injector.cpp
│   ├── main.cpp                wmain entry point
│   ├── process.cpp
│   └── unique_handle.hpp       Internal RAII handle wrapper
└── Source/                     Visual Studio solution and project
```

## 🚀 Getting Started :

### Visual Studio :

1. Open the solution file (`.sln`).
2. Build the project in Release (x86) or Release (x64).

Every configuration in x86 / x64 (Debug and Release) is already configured.

> **Warning** <br>
> The injector architecture must match the target process's architecture. An x86 injector cannot inject a 64-bit process and vice versa. The program detects a mismatch and reports it.

### Other IDE using CMake :

A `CMakeLists.txt` is provided at the root of the repository:

```bash
cmake -S . -B build
cmake --build build --config Release
```

Tested on CLion with the MSVC compiler, you can get the Visual Studio Build Tools [**here**](https://visualstudio.microsoft.com/fr/downloads/?q=build+tools).

## 🧪 Usage :

### How to use the program :

Use it in the command line :

    inject.exe <dll> <process>

- `<dll>` : path to the DLL to inject (absolute or relative).
- `<process>` : target executable name (e.g. `notepad.exe`) or a numeric PID.

    inject.exe test.dll notepad.exe
    inject.exe test.dll 1234

Use `-h` / `--help` to display the usage.

### Exit codes :

| Code | Meaning                        |
|-----:|--------------------------------|
| 0    | Success                        |
| 1    | Usage error                    |
| 2    | Process not found              |
| 3    | DLL path could not be resolved |
| 4    | Injection failed               |

**You can get the DLL injector (x86 / x64) and the test files (x86 / x64) in the "Release" section.**

### Limitations :

- The injector uses `LoadLibrary`, which is the most detectable injection technique; security products commonly flag it.
- On 64-bit builds, the module handle returned by `LoadLibrary` is truncated to 32 bits by the remote thread exit code. The injection result (success or failure) is still reliable.

### Demonstration :

### TEST FILES (x86)

https://user-images.githubusercontent.com/48086737/170119196-8610ee0b-984e-4a42-a931-007dccd3d8c0.mp4
