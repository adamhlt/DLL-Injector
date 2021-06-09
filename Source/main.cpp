#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

//Récupération du PID à partir du nom du processus
int getProcessByName(const std::string& processName)
{
    int iProcID = NULL;
    PROCESSENTRY32 procList;
    procList.dwSize = sizeof(procList);

    HANDLE snapProcList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapProcList == INVALID_HANDLE_VALUE) return 0;

    if (!Process32First(snapProcList, &procList)) return 0;
    if (std::string(procList.szExeFile) == processName) iProcID = procList.th32ProcessID;
    else
    {
        while (Process32Next(snapProcList, &procList))
        {
            if (std::string(procList.szExeFile) == processName) iProcID = procList.th32ProcessID;
        }
        if (iProcID == NULL) return 0;
    }
    return iProcID;
}

int main(int argc, char *argv[]) {
    std::string sDllName;
    std::string sDllPath;
    std::string sProcName;
    char cPathBuffer[MAX_PATH];
    if (argc == 3)
    {
        sDllName = argv[1];
        sProcName = argv[2];
    }
    else
    {
        std::cout << "[HELP] inject.exe <dll> <process>" << std::endl;
        return -1;
    }

    //Récupération du PID
    int iPID = getProcessByName(sProcName);
    if (iPID == 0) return -1;
    std::cout << "\n[INFORMATION]\n" << std::endl;
    std::cout << "Process : " << sProcName << std::endl;
    std::cout << "Process PID : " << iPID << std::endl;

    //Récupération du chemin complet du dll
    if (GetFullPathNameA(sDllName.c_str(), MAX_PATH, cPathBuffer, nullptr) == 0) return -1;
    sDllPath = cPathBuffer;
    std::cout << "DLL Path : " << sDllPath << std::endl;

    //Ouverture du processus pour effectuer les opérations d'allocation de mémoire et d'écriture
    HANDLE hDestProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, iPID);
    if (hDestProc == INVALID_HANDLE_VALUE) return -1;
    std::cout << "\n[INJECTION PROCESS]\n" << std::endl;
    std::cout << "Process opened sucessfully !" << std::endl;

    //Allocation de mémoire pour écrire le chemin du dll
    LPVOID lPathAddr = VirtualAllocEx(hDestProc, nullptr, sDllPath.length(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (lPathAddr == nullptr) return -1;
    std::cout << "DLL Path write at : " << lPathAddr << std::endl;

    //Écriture du chemin du dll
    int iWriteResult = WriteProcessMemory(hDestProc, lPathAddr, sDllPath.c_str(), sDllPath.length(), nullptr);
    if(iWriteResult == 0) return -1;
    std::cout << "DLL path writed successfully !" << std::endl;

    //Récupération de l'adresse de la fonction "LoadLibraryA"
    FARPROC fFuncAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (fFuncAddr == nullptr) return -1;
    std::cout << "LoadLibraryA Address : " << (LPVOID)fFuncAddr << std::endl;

    //Création d'un nouveau thread qui appele la fonction "LoadLibraryA" et qui prend comme paramètre le chemin du dll
    HANDLE iThreadResult = CreateRemoteThread(hDestProc, nullptr, 0, (LPTHREAD_START_ROUTINE)fFuncAddr, lPathAddr, 0, nullptr);
    if (iThreadResult == INVALID_HANDLE_VALUE) return -1;
    std::cout << "DLL injected !\n" << std::endl;

    system("PAUSE");

    return 0;
}