# DLL Injector [![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=plastic)](https://en.wikipedia.org/wiki/C%2B%2B) [![Windows](https://img.shields.io/badge/platform-Windows-0078d7.svg)](https://en.wikipedia.org/wiki/Microsoft_Windows) [![x86](https://img.shields.io/badge/arch-x86-red.svg)](https://en.wikipedia.org/wiki/X86) [![x86](https://img.shields.io/badge/arch-x64-green.svg)](https://en.wikipedia.org/wiki/X64)
### Présentation
Ceci est un injecteur de DLL écrit en C++ . Celui-ci est sous sa forme la plus basique, en utilisant la fonction "LoadLibraryA" pour charger le DLL.

Les commentaires dans les sources n'expliquent pas en détail le fonctionnement l'injection de DLL. 

### Utilisation
Cet injecteur s'utilise en ligne de commande de façon très simple.

    inject.exe <dll> <process>
Vous pouvez tester l'injecteur avec les fichiers fournis dans le dossier "Release", les deux architectures (x86 et x64) sont présentes.

    inject.exe test.dll test.exe
La commande ci-dessus vous permet de tester l'injecteur avec les fichiers fournis.
### Démonstration
![Démontration de l'injecteur de DLL.](https://github.com/adamhlt/DLL-Injector/blob/main/Ressource/demo.gif)
