#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdexcept>
#include <sstream>
#include <ostream>
#include <fstream>
#include <stdio.h>
#include <Shlwapi.h>

#include "XorStr.h"
#include "Utils.h"
#include "MAC.h"

//#include "FilesChecker.h"
#include "Shaders.h"

//int FileSize = 1101520;
//int ShaderSize = 102585386;

static int Retries = 0;

unsigned long WINAPI Init(void* Instance)
{
    while (!(GetModuleHandleA(XorStr("BypassBE.dll"))))
        Sleep(200);

    std::stringstream SS;
    std::stringstream SS1;

    static TCHAR Path[MAX_PATH];
    std::string Folder, File;

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, Path)))
    {
        Folder = std::string(Path) + "\\EFTGhost\\";
        File = std::string(Path) + "\\EFTGhost\\cfg.ini";
    }

    g_Config->Load();

    try
    {
        Utils::CreateConsole(XorStr(" "));

        SS << XorStr("[LOG] Attached at PID: ") << GetCurrentProcessId() << XorStr("\n");
        printf(SS.str().c_str());

        Utils::WaitForInputInConsole();

        Utils::Cleaner();

        printf(XorStr("[LOG] Asking for file path with dialog. \n"));

        Utils::BrowseFolders(NULL, XorStr("C:\\"), NULL);

        SS1 << XorStr("[LOG] Current tarkov path is ") << XorStr("'") << Globals::TarkovPath << XorStr("'") << XorStr("\n");
        printf(SS1.str().c_str());

        Utils::WaitForInputInConsole();

CheckAgain:
        if (!Utils::FileExist(File))
        {
            Utils::AutoUpdate(false); 
        }
        else
        {
            if (Utils::CheckIfAlreadyInstalled() != 1)
            {
                printf(XorStr("[ERROR] Please run a file integrity check. \n"));
                Utils::WaitForInputInConsole();
                Retries++;
            }

            if (Retries == 2)
            {
                system("cls");
                Utils::AutoUpdate(true);
            }
        }

        std::string ShaderFilePath = Globals::TarkovPath + XorStr("\\EscapeFromTarkov_Data\\StreamingAssets\\Windows\\shaders");

        if (Utils::CheckIfAlreadyInstalled() == 2 || Utils::GetFileSizeBytes(ShaderFilePath.c_str()) != Vars.OriginalFileSize)
        {
            if (Retries == 2)
            {
                Retries = 0;
            }
            goto CheckAgain;
        }

        g_Config->Load();

        printf(XorStr("[LOG] Implementing Anti Anti-Cheat in 1 second. \n"));
        Sleep(1000);







        // removed because this still works







        Utils::WaitForInputInConsole();
        Utils::ClearLogsAndCache();
        Utils::ClickOnPlay();

        Sleep(250);
        printf( XorStr( "[LOG] Implementing custom models in 1 second. \n" ) );

        if (Utils::GetFileSizeBytes(ShaderFilePath.c_str()) == Vars.OriginalFileSize) {
            std::ofstream(Globals::TarkovPath + "\\EscapeFromTarkov_Data\\StreamingAssets\\Windows\\shaders", std::ios::binary).write((const char*)PatchedShaders, sizeof(PatchedShaders));
            static int CurrentFileSize = Utils::GetFileSizeBytes(ShaderFilePath.c_str());
            Utils::ChangeFileSize(ShaderFilePath.c_str(), Vars.OriginalFileSize - CurrentFileSize);
        }
        else {
            MessageBoxA(nullptr, XorStr("Invalid heap size 0x2!"), XorStr("Error!"), MB_OK);
        }

        if (Utils::GetFileSizeBytes(ShaderFilePath.c_str()) == Vars.OriginalFileSize) {
            Sleep(20000);
        }
        else {
            Utils::ElevatedTerminate(XorStr("EscapeFromTarkov.exe"));
            Utils::ElevatedTerminate(XorStr("EscapeFromTarkov_BE.exe"));
            Utils::ElevatedTerminate(XorStr("BEService_x64.exe"));
            MessageBoxA(nullptr, XorStr("Critical error occured closing all related processes!."), XorStr("Error!"), MB_OK);
            exit(EXIT_FAILURE);
        }

        printf(XorStr("[LOG] Done. \n"));
        Globals::Unload = true;
    }
	catch (const std::runtime_error& Error) 
	{
		MessageBoxA(nullptr, XorStr(Error.what()), XorStr("Error!"), MB_OK | MB_ICONERROR);
		FreeLibraryAndExitThread(static_cast<HMODULE>(Instance), 0);
	}

    while (!Globals::Unload)
        Sleep(50);

    Utils::DestroyConsole();

    FreeLibraryAndExitThread(static_cast<HMODULE>(Instance), 0);
}

BOOL APIENTRY DllMain(HMODULE Module, DWORD Reason, LPVOID Reserved)
{
    DisableThreadLibraryCalls(Module);

    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
        if (auto Handle = CreateThread(nullptr, NULL, Init, Module, NULL, nullptr))
            CloseHandle(Handle);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

