<div align="center">

# DLL Injector

**LoadLibrary DLL injector in C++ (x86 / x64)**

![Language](https://img.shields.io/badge/C%2B%2B-16171C?logo=cplusplus)
![Platform](https://img.shields.io/badge/Windows-16171C)
![Arch](https://img.shields.io/badge/x86%20%7C%20x64-16171C)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-16171C)](LICENSE)

</div>

---

DLL Injector loads a DLL into a running Windows process using the most basic technique: a remote thread that calls `LoadLibraryW`.

The code is split into a small injector library and a thin command-line front end, so you can use it as a tool or reuse the library in your own project.

### Example
https://user-images.githubusercontent.com/48086737/170119196-8610ee0b-984e-4a42-a931-007dccd3d8c0.mp4

## Features

**Targets**
- x86 and x64, Debug and Release
- Target a process by executable name or by PID
- Test programs (x86 / x64) available in the "Release" section

**Injection**
- Waits for the remote thread and reports the injection outcome
- Detects an architecture mismatch between the injector and the target
- Unicode entry point and wide Win32 APIs, with readable error messages
- Distinct exit codes for scripting

**Code**
- Reusable library in `include/injector/` and `src/`
- RAII handle wrapper, no leaked handles
- Visual Studio solution

## Usage

```
inject.exe <dll> <process>
```

- `<dll>`: path to the DLL to inject (absolute or relative).
- `<process>`: target executable name (e.g. `notepad.exe`) or a numeric PID.

```
inject.exe test.dll notepad.exe
inject.exe test.dll 1234
```

Use `-h` / `--help` to display the usage.

> [!WARNING]
> The injector architecture must match the target process's architecture. An x86 injector cannot inject a 64-bit process and vice versa. The program detects a mismatch and reports it.

### Exit codes

| Code | Meaning                        |
|-----:|--------------------------------|
| 0    | Success                        |
| 1    | Usage error                    |
| 2    | Process not found              |
| 3    | DLL path could not be resolved |
| 4    | Injection failed               |

### Limitations

- `LoadLibrary` is the most detectable injection technique; security products commonly flag it.
- On 64-bit builds, the module handle returned by `LoadLibrary` is truncated to 32 bits by the remote thread exit code. The injection result (success or failure) is still reliable.

## Build

1. Open `DLL Injector.sln`.
2. Build in Release (x86 or x64).

Every configuration is already set up.

## License

This project is licensed under the [GNU GPLv3](LICENSE).
