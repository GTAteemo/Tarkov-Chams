#pragma once
#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <conio.h>
#include <TlHelp32.h>
#include <fstream>
#include <random>

#include "Variables.h"
#include "XorStr.h"
#include "Globals.h"
#include "MD5.h"
#include "MAC.h"
#include "Config.h"

namespace Utils {
    inline void CreateConsole(const char* Title) 
    {
        AllocConsole();
        freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(0)), XorStr("conin$"), XorStr("r"), static_cast<_iobuf*>(__acrt_iob_func(0)));
        freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(1)), XorStr("conout$"), XorStr("w"), static_cast<_iobuf*>(__acrt_iob_func(1)));
        freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(2)), XorStr("conout$"), XorStr("w"), static_cast<_iobuf*>(__acrt_iob_func(2)));
        SetConsoleTitleA(Title);
    }

    inline void DestroyConsole() 
    {
        fclose(static_cast<_iobuf*>(__acrt_iob_func(0)));
        fclose(static_cast<_iobuf*>(__acrt_iob_func(1)));
        fclose(static_cast<_iobuf*>(__acrt_iob_func(2)));
        FreeConsole();
    }

    inline void WaitForInputInConsole() 
    {
        system(XorStr("pause"));
        system(XorStr("cls"));
    }

    inline bool DeleteFileNow(const wchar_t* filename)
    {
        if (!PathFileExistsW(filename))
            return false;

        wchar_t path[MAX_PATH];
        wcscpy_s(path, filename);
        PathRemoveFileSpecW(path);

        wchar_t tempname[MAX_PATH];
        if (!GetTempFileNameW(path, L".xX", 0, tempname))
            return false;

        if (!MoveFileExW(filename, tempname, MOVEFILE_REPLACE_EXISTING))
        {
            DeleteFileW(tempname);
            return false;
        }

        return DeleteFileW(tempname) != FALSE;
    }

    inline int CheckIfAlreadyInstalled()
    {
        //std::ifstream FilesCheckerFile;
        //FilesCheckerFile.open(Globals::TarkovPath + "\\EscapeFromTarkov_Data\\Managed\\FilesChecker.dll", std::ios::binary | std::ios::in);
        //FilesCheckerFile.seekg(0, std::ios::end);
        //long Length = FilesCheckerFile.tellg();
        //FilesCheckerFile.seekg(0, std::ios::beg);
        //char* InFileData = new char[Length];
        //FilesCheckerFile.read(InFileData, Length);
        //std::string Temp = md5(InFileData, Length);

        std::ifstream ShadersFile;
        ShadersFile.open(Globals::TarkovPath + "\\EscapeFromTarkov_Data\\StreamingAssets\\Windows\\shaders", std::ios::binary | std::ios::in);
        ShadersFile.seekg(0, std::ios::end);
        long Length1 = ShadersFile.tellg();
        ShadersFile.seekg(0, std::ios::beg);
        char* InFileData1 = new char[Length1];
        ShadersFile.read(InFileData1, Length1);
        std::string Temp1 = md5(InFileData1, Length1);

        Vars.CurrentHash = Temp1;

        if (/*Temp == "27058f062fb1d293983278d8d345eec1" || */ Temp1 == Vars.NewHash)
            return 1;
        else
            return 2;

        return 0;
    }

    inline std::string CurrentHash()
    {
        std::ifstream ShadersFile;
        ShadersFile.open(Globals::TarkovPath + "\\EscapeFromTarkov_Data\\StreamingAssets\\Windows\\shaders", std::ios::binary | std::ios::in);
        ShadersFile.seekg(0, std::ios::end);
        long Length1 = ShadersFile.tellg();
        ShadersFile.seekg(0, std::ios::beg);
        char* InFileData1 = new char[Length1];
        ShadersFile.read(InFileData1, Length1);
        std::string Temp1 = md5(InFileData1, Length1);

        return Temp1;
    }

    inline int GetFileSizeBytes(const char* FileName)
    {
        std::ifstream FileToRead;
        FileToRead.open(FileName, std::ios_base::binary);
        FileToRead.seekg(0, std::ios_base::end);
        int Size = FileToRead.tellg();
        FileToRead.close();
        return Size;
    }

    inline void AutoUpdate(bool HasConfig)
    {
        static int UpdateChoice = -1;
        static int ConfirmChoice = -1;
        static int ConfirmChoice1 = -1;

        if (HasConfig)
        {
            printf(XorStr("[BETA] Would you like to try to update? Type 1 for yes | Type 2 for no. \n"));
            std::cin >> UpdateChoice;
        }
        else
            UpdateChoice = 1;

        if (UpdateChoice == 1)
        {
            system("cls");

            std::string ShaderFilePath = Globals::TarkovPath + XorStr("\\EscapeFromTarkov_Data\\StreamingAssets\\Windows\\shaders");
            
InputFileSize:
            printf(XorStr("[BETA] Current shaders size %d. \n"), GetFileSizeBytes(ShaderFilePath.c_str()));
            printf(XorStr("[BETA] Please enter the file size of the legit shaders file. (Ex 100000000). \n"));
            std::cin >> Vars.OriginalFileSize;

            if (Vars.OriginalFileSize != GetFileSizeBytes(ShaderFilePath.c_str()))
            {
                printf(XorStr("[BETA] Are you sure the file size you entered is correct? Type 1 for yes | Type 2 for no. \n"));
                std::cin >> ConfirmChoice;

                if (ConfirmChoice != 1)
                {
                    Vars.OriginalFileSize = 0;
                    system("cls");
                    goto InputFileSize;
                }
            }

            printf(XorStr("[BETA] Updated file size its now %d. \n"), Vars.OriginalFileSize);
            Sleep(500);
            system("cls");

InputHash:
            std::cout << "[BETA] Current shaders hash " << CurrentHash() << ". \n";
            printf(XorStr("[BETA] Please enter the hash of the legit shaders file. \n"));

            std::cin >> Vars.NewHash;

            if (Vars.NewHash != CurrentHash())
            {
                printf(XorStr("[BETA] Are you sure the hash you entered is correct? Type 1 for yes | Type 2 for no. \n"));
                std::cin >> ConfirmChoice1;

                if (ConfirmChoice1 != 1)
                {
                    Vars.NewHash = "";
                    system("cls");
                    goto InputHash;
                }
            }

            std::cout << "[BETA] Updated shaders hash its now " << Vars.NewHash << ". \n";
            Sleep(500);
            system("cls");

            g_Config->Save();
        }
        else
        {
            system("cls");
        }
    }

    inline void ChangeFileSize(const char* File, int BytesToAdd)
    {
        std::ofstream fout;
        fout.open(File, std::ios::app | std::ios::binary);

        std::random_device RandomDevice;
        std::mt19937 Generate(RandomDevice());
        std::uniform_int_distribution<> Distribute(1, 255);

        for (int i = 0; i < BytesToAdd; i++) {
            char RandomByte = (char)Distribute(Generate);
            fout.write(&RandomByte, sizeof(RandomByte));
        }

        fout.close();
    }

    inline void Cleaner()
    {
        static int CleanerChoice = -1;
        printf(XorStr("[BETA] Would you like to clean traces? Type 1 for yes | Type 2 for no. \n"));
        std::cin >> CleanerChoice;

        if (CleanerChoice == 1)
        {
            system("cls");
            std::cout << "[WARNING] This will not prevent BattleEye trace bans. This will only prevent BatteState Games Launcher trace bans. \n";
            system("pause");
            system("cls");

            static int MACChoice = -1;
            std::cout << "Would you like to spoof your MAC address? Type 1 for yes | type 2 for no. \n";
            std::cin >> MACChoice;

            if (MACChoice == 1)
            {
                MAC* g_Mac = new MAC();

                g_Mac->GenRandomMAC();
                g_Mac->AssingRandomMAC();

                system("cls");
                goto DeleteBEService;
            }
            else
            {
                system("cls");
                goto DeleteBEService;
            }
        DeleteBEService:
            static int DeleteBEChoice = -1;
            std::cout << "Would you like to delete BEService? Type 1 for yes | type 2 for no. \n";
            std::cin >> DeleteBEChoice;

            if (DeleteBEChoice == 1)
            {
                system("reg delete HKLM\\SYSTEM\\ControlSet001\\Services\\BEService /f");
                system("cls");
                goto ClearTemp;
            }
            else
            {
                system("cls");
                goto ClearTemp;
            }
        ClearTemp:
            static int ClearTempChoice = -1;
            std::cout << "Would you like to clear your temp? Type 1 for yes | type 2 for no. \n";
            std::cin >> ClearTempChoice;

            if (ClearTempChoice == 1)
            {
                system("rmdir /s /q %systemdrive%\\Users\\%username%\\AppData\\Local\\Temp");
                system("cls");
                goto ClearPrefetch;
            }
            else
            {
                system("cls");
                goto ClearPrefetch;
            }
        ClearPrefetch:
            static int ClearPrefetchChoice = -1;
            std::cout << "Would you like to clear your prefetch? Type 1 for yes | type 2 for no. \n";
            std::cin >> ClearPrefetchChoice;

            if (ClearPrefetchChoice == 1)
            {
                system("rmdir /s /q %systemdrive%\\Windows\\Prefetch");
                system("cls");
                goto ClearD3DSCache;
            }
            else
            {
                system("cls");
                goto ClearD3DSCache;
            }
        ClearD3DSCache:
            static int ClearD3DSCacheChoice = -1;
            std::cout << "Would you like to clear your D3DS cache? Type 1 for yes | type 2 for no. \n";
            std::cin >> ClearD3DSCacheChoice;

            if (ClearD3DSCacheChoice == 1)
            {
                system("rmdir /s /q %systemdrive%\\Users\\%username%\\AppData\\Local\\D3DSCache");
                system("cls");
                goto GenerateRandomProductID;
            }
            else
            {
                system("cls");
                goto GenerateRandomProductID;
            }
        GenerateRandomProductID:
            static int GenerateRandomProductIDChoice = -1;
            std::cout << "Would you like to generate new product ID? Type 1 for yes | type 2 for no. \n";
            std::cin >> GenerateRandomProductIDChoice;

            if (GenerateRandomProductIDChoice == 1)
            {
                system("REG ADD HKLM\\SOFTWARE\\Microsoft\\Windows\" \"NT\\CurrentVersion /v ProductId /t REG_SZ /d %random%%random%-%random%-%random%-%random% /f");
                system("cls");
                CleanerChoice = -1;
            }
            else
            {
                system("cls");
                CleanerChoice = -1;
            }
        }
        else
        {
            system("cls");
        }
    }

    inline void ClickOnPlay() 
    {
        HWND Window = FindWindowA(NULL, XorStr("BsgLauncher"));
        RECT WindowRect;

        SetForegroundWindow(Window);
        Sleep(100);
        ShowWindow(Window, SW_RESTORE);
        Sleep(100);
        SetWindowPos(Window, NULL, 0, 0, 1150, 680, NULL);
        Sleep(100);
        GetWindowRect(Window, &WindowRect);
        Sleep(100);
        SetCursorPos(WindowRect.left + 950, WindowRect.top + 570);
        Sleep(100);
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        Sleep(100);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }

    inline int DeleteDirectory(const std::string& Directory, bool DeleteSubDirs = true)
    {                                                  
        HANDLE File;                       
        std::string StringFilePath;               
        std::string StringPattern;
        WIN32_FIND_DATA FileInformation;   
        bool Subdirectory = false;

        StringPattern = Directory + "\\*.*";
        File = ::FindFirstFile(StringPattern.c_str(), &FileInformation);
        if (File != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (FileInformation.cFileName[0] != '.')
                {
                    StringFilePath.erase();
                    StringFilePath = Directory + "\\" + FileInformation.cFileName;

                    if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        if (DeleteSubDirs)
                        {
                            int iRC = DeleteDirectory(StringFilePath, DeleteSubDirs);
                            if (iRC)
                                return iRC;
                        }
                        else
                            Subdirectory = true;
                    }
                    else
                    {
                        if (::SetFileAttributes(StringFilePath.c_str(),
                            FILE_ATTRIBUTE_NORMAL) == FALSE)
                            return ::GetLastError();

                        if (::DeleteFile(StringFilePath.c_str()) == FALSE)
                            return ::GetLastError();
                    }
                }
            } while (::FindNextFile(File, &FileInformation) == TRUE);

            ::FindClose(File);

            DWORD dwError = ::GetLastError();
            if (dwError != ERROR_NO_MORE_FILES)
                return dwError;
            else
            {
                if (!Subdirectory)
                {
                    if (::SetFileAttributes(Directory.c_str(),
                        FILE_ATTRIBUTE_NORMAL) == FALSE)
                        return ::GetLastError();

                    if (::RemoveDirectory(Directory.c_str()) == FALSE)
                        return ::GetLastError();
                }
            }
        }

        return 0;
    }

    inline void ClearLogsAndCache()
    {
        DeleteDirectory(Globals::TarkovPath + "\\Logs");
        DeleteDirectory(Globals::TarkovPath + "\\cache");
    }

    inline void CheckPrivileges() 
    {
        bool IsRunningAsAdmin = false;

        BOOL Ret = FALSE;
        HANDLE Token = NULL;

        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &Token)) 
        {
            TOKEN_ELEVATION Elevation;
            DWORD Size = sizeof(TOKEN_ELEVATION);
            if (GetTokenInformation(Token, TokenElevation, &Elevation, sizeof(Elevation), &Size)) {

                Ret = Elevation.TokenIsElevated;
            }
        }

        if (Token) 
        {
            CloseHandle(Token);
        }

        IsRunningAsAdmin = Ret;

        if (!IsRunningAsAdmin) 
        {
            MessageBoxA(NULL, (LPCSTR)XorStr("Please run the BsgLauncher as admin."), (LPCSTR)XorStr("Error!"), MB_OK);
            exit(EXIT_FAILURE);
        }
    }

    inline int CALLBACK BrowseForFolderCallback(HWND hwnd, UINT MSG, LPARAM LParam, LPARAM Data) 
    {
        char Path[MAX_PATH];

        switch (MSG) {
        case BFFM_INITIALIZED:
            SendMessage(hwnd, BFFM_SETSELECTION, TRUE, Data);
            break;
        case BFFM_SELCHANGED:
            if (SHGetPathFromIDList((LPITEMIDLIST)LParam, Path)) 
            {
                SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)Path);

            }
            break;
        }

        return 0;
    }

    inline BOOL BrowseFolders(HWND hwnd, LPSTR Folder, LPSTR Title) 
    {
        BROWSEINFO BrowserInfo;
        char Path[MAX_PATH + 1];
        LPITEMIDLIST PIDL;
        BOOL Result = FALSE;
        LPMALLOC Malloc;

        if (SUCCEEDED(SHGetMalloc(&Malloc))) 
        {
            BrowserInfo.hwndOwner = hwnd;
            BrowserInfo.pidlRoot = NULL;
            BrowserInfo.pszDisplayName = NULL;
            BrowserInfo.lpszTitle = Title;
            BrowserInfo.ulFlags = BIF_STATUSTEXT;
            BrowserInfo.lpfn = BrowseForFolderCallback;
            BrowserInfo.lParam = (LPARAM)Folder;

            PIDL = SHBrowseForFolder(&BrowserInfo);

            if (PIDL) 
            {
                if (SHGetPathFromIDList(PIDL, Path)) 
                {
                    Result = TRUE;
                    strcpy(Folder, Path);

                    Globals::TarkovPath = Path;
                }

                Malloc->Free(PIDL);
                Malloc->Release();
            }
        }

        return Result;
    }

    inline DWORD FindProcessId(const std::string& ProcessName)
    {
        PROCESSENTRY32 ProcessEntry;
        ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (Process32First(Snapshot, &ProcessEntry) == TRUE)
        {
            while (Process32Next(Snapshot, &ProcessEntry) == TRUE)
            {
                if (_stricmp(ProcessEntry.szExeFile, ProcessName.c_str()) == 0)
                {
                    HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessEntry.th32ProcessID);
      
                    CloseHandle(Process);
                    return ProcessEntry.th32ProcessID;
                }
            }
        }

        CloseHandle(Snapshot);
    }

    inline void Suspend(DWORD ProcessId)
    {
        HANDLE ThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

        THREADENTRY32 ThreadEntry;
        ThreadEntry.dwSize = sizeof(THREADENTRY32);

        Thread32First(ThreadSnapshot, &ThreadEntry);

        do
        {
            if (ThreadEntry.th32OwnerProcessID == ProcessId)
            {
                HANDLE Thread = OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadEntry.th32ThreadID);

                SuspendThread(Thread);
                CloseHandle(Thread);
            }
        } while (Thread32Next(ThreadSnapshot, &ThreadEntry));

        CloseHandle(ThreadSnapshot);
    }

    inline void Resume(DWORD ProcessId)
    {
        HANDLE ThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

        THREADENTRY32 ThreadEntry;
        ThreadEntry.dwSize = sizeof(THREADENTRY32);

        Thread32First(ThreadSnapshot, &ThreadEntry);

        do
        {
            if (ThreadEntry.th32OwnerProcessID == ProcessId)
            {
                HANDLE Thread = OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadEntry.th32ThreadID);

                ResumeThread(Thread);
                CloseHandle(Thread);
            }
        } while (Thread32Next(ThreadSnapshot, &ThreadEntry));

        CloseHandle(ThreadSnapshot);
    }

    inline bool IsRunning(LPCSTR Name)
    {
        HWND hwnd;
        hwnd = FindWindow(NULL, Name);
        if (hwnd != 0) {
            return true;
        }
        else {
            return false;
        }
    }

    inline bool FileExist(const std::string& name) 
    {
        if (FILE* file = fopen(name.c_str(), "r")) 
        {
            fclose(file);
            return true;
        }
        else 
        {
            return false;
        }
    }
};