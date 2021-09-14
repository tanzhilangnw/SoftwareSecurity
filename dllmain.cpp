// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "framework.h"
#include "detours.h"
#include "stdio.h"
#include "stdarg.h"
#include "windows.h"
#include <fstream>
#include <iostream>
#include "json.hpp"
#include "stdlib.h"
#include <vector>
#include <map>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma comment(lib,"detours.lib")
using json = nlohmann::json;
using namespace std;    
#define MESSAGEBOXA 1
#define MESSAGEBOXW 2
#define CREATEFILE 3
#define WRITEFILE 4
#define READFILE 5

#define HEAPCREATE 6
#define HEAPDESTORY 7
#define HEAPFREE 8

#define REGCREATEKEYEX 9
#define REGSETVALUEEX 10
#define REGCLOSEKEY 11
#define REGOPENKEYEX 12
#define REGDELETEVALUE 13

#define THESOCKET 14
#define BIND 15
#define SEND 16
#define CONNECT 17
#define RECV 18


SYSTEMTIME st;

//HANDLE hSemaphore = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, L"mySemaphore");
//HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, L"ShareMemory");
//LPVOID lpBase = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(info));

static int (WINAPI* OldMessageBoxW)(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType) = MessageBoxW;
static int (WINAPI* OldMessageBoxA)(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType) = MessageBoxA;

extern "C" __declspec(dllexport)int WINAPI NewMessageBoxA(_In_opt_ HWND hWnd, _In_opt_ LPCSTR lpText, _In_opt_ LPCSTR lpCaption, _In_ UINT uType) {
    GetLocalTime(&st);
    char send_info[4][80];
    char time[80] = { 0 };


    //参数值
    sprintf_s(send_info[0], "%08X", hWnd);
    sprintf_s(send_info[1], "%s", lpText);
    sprintf_s(send_info[2], "%s", lpCaption);
    sprintf_s(send_info[3], "%08X", uType);
    sprintf_s(time,"%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    
    
    json info = {{"type","MESSAGEBOXA"},
        {"time",time},
        {"arg",
        {"hWnd",send_info[0]},
        {"lpText",send_info[1]},
        {"lpCaption",send_info[2]},
        {"uType",send_info[3]}
         }
    };
    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();
    return OldMessageBoxA(NULL, "new MessageBoxA", "Hooked", MB_OK);
}

extern "C" __declspec(dllexport)int WINAPI NewMessageBoxW(_In_opt_ HWND hWnd, _In_opt_ LPCWSTR lpText, _In_opt_ LPCWSTR lpCaption, _In_ UINT uType) {
    char send_info[4][80];
    char time[80];
    GetLocalTime(&st);
    sprintf_s(send_info[0], "%08X", hWnd);
    sprintf_s(send_info[1], "%s", lpText);
    sprintf_s(send_info[2], "%s", lpCaption);
    sprintf_s(send_info[3], "%08X", uType);
    sprintf_s(time,"%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    json info = {
        {"type","MESSAGEBOXW"},
        {"time",time},
        {"arg",
        {"hWnd",send_info[0]},
        {"lpText",send_info[1]},
        {"lpCaption",send_info[2]},
        {"uType",send_info[3]}
         }
    };
    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();

    return OldMessageBoxW(NULL, L"new MessageBoxW", L"Hooked", MB_OK);
}

static HANDLE(WINAPI* OldCreateFile)(
    LPCTSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    ) = CreateFile;

extern "C" __declspec(dllexport)HANDLE WINAPI NewCreateFile(
    LPCTSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
)

{
    char time[80];
    char send_info[7][80];
    printf("CreateFile Hooked\n");
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    sprintf_s(send_info[0],"%s",lpFileName);
    sprintf_s(send_info[1], "%08X", dwDesiredAccess);
    sprintf_s(send_info[2], "%08X", dwShareMode);
    sprintf_s(send_info[3], "%08X", lpSecurityAttributes);
    sprintf_s(send_info[4], "%08X", dwCreationDisposition);
    sprintf_s(send_info[5], "%08X", dwFlagsAndAttributes);
    sprintf_s(send_info[6], "%08X", hTemplateFile);



    json info = { 
        {"type","CREATEFILE"},
        {"time",time},
        {"arg",{"lpFileName",send_info[0]},
        {"dwDesiredAccess",send_info[1]},
        {"dwShareMode",send_info[2]},
        {"lpSecurityAttributes",send_info[3]},
        {"dwCreationDisposition",send_info[4]},
        {"dwCreationDisposition",send_info[5]},
        {"hTemplateFile",send_info[6]}
    }
    };
    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();
    //memcpy(lpBase, &sendInfo, sizeof(info));
    return OldCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


static BOOL(WINAPI* OldWriteFile)(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesToWritten,
    LPOVERLAPPED lpOverLapped
    ) = WriteFile;

extern "C" __declspec(dllexport)BOOL WINAPI NewWriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesToWritten,
    LPOVERLAPPED lpOverLapped
) 

{
    char send_info[5][80];
    char time[80];
    //参数值
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    sprintf_s(send_info[0], "%08X", hFile);
    sprintf_s(send_info[1],"%08X", lpBuffer);
    sprintf_s(send_info[2], "%08X",nNumberOfBytesToWrite);
    sprintf_s(send_info[3], "%08X",lpNumberOfBytesToWritten);
    sprintf_s(send_info[4], "%08X",lpOverLapped);

    json info = { 
               {"type","WriteFile"},
               {"time",time},
               {"arg",
               {"hFile",send_info[0]},
               {"lpBuffer",send_info[1]},
               {"nNumberofBytesToWrite",send_info[2]},
               {"lpNUmberOfBytesToWritten",send_info[3]},
               {"lpOverLapped",send_info[5]}
                }
    };
    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();

    return OldWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesToWritten, lpOverLapped);
}


static BOOL(WINAPI* OldReadFile)(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    ) = ReadFile;

extern "C" __declspec(dllexport)BOOL WINAPI NewReadFile(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
)
{
    char send_info[5][80];
    char time[80];
    // 参数值
    sprintf_s(send_info[0], "%08X", hFile);
    sprintf_s(send_info[1], "%08X", lpBuffer);
    sprintf_s(send_info[2], "%08X", nNumberOfBytesToRead);
    sprintf_s(send_info[3], "%08X", lpNumberOfBytesRead);
    sprintf_s(send_info[4], "%08X", lpOverlapped);
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);

    json info = {
        {"type","READFILE"},
        {"time",time},
        {"arg",
        {"hFile",send_info[0]},
        {"lpBuffer",send_info[1]},
        {"nNumberOfBytesToRead",send_info[2]},
        {"lpNumberOfBytesRead",send_info[3]},
        {"lpOverlapped",send_info[4]}}
    };
    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();
    return OldReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

static std::map<HANDLE,bool> Heap;
static int freenumber = 0,allocnumber = 0;

static HANDLE(WINAPI* OldHeapCreate)(DWORD fIOoptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) = HeapCreate;
extern "C" __declspec(dllexport)HANDLE WINAPI NewHeapCreate(DWORD fIOoptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
    HANDLE hHeap = OldHeapCreate(fIOoptions, dwInitialSize, dwMaximumSize);
    Heap.insert(pair<HANDLE,bool>(hHeap,0));
    char time[80];
    char send_info[4][80];
    //参数值
    sprintf_s(send_info[0],"%08X",fIOoptions);
    sprintf_s(send_info[1],"%08X",dwInitialSize);
    sprintf_s(send_info[2],"%08X",dwMaximumSize);
    sprintf_s(send_info[3],"%08X",hHeap);
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore,1,NULL);
    json info = {
        {"type","HeapCreate"},
        {"time",time},
        {"arg",{"fIOoptions",send_info[0]},
        {"dwInitialSize",send_info[1]},
        {"dwMaximumSize",send_info[2]},
        {"HANDLE",send_info[3]}},
    };

    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();

    return hHeap;
}

static BOOL(WINAPI* OldHeapDestroy)(HANDLE) = HeapDestroy;
extern "C" __declspec(dllexport)BOOL WINAPI NewHeapDestroy(HANDLE hHeap) {
    
    char send_info[1][80];
    char time[80];
    //参数值
    sprintf_s(send_info[0],"%08X",hHeap);
    printf("HeapDestroy Hooked\n");
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    //memcpy(lpBase,&sendInfo,sizeof(info));
    //ReleaseSemaphore(hSemaphore,1,NULL);
    json info = {
       {"type","HEAPDESTROY"},
       {"time",time},
       {"arg",
       {"HANDLE",send_info[0]},
       {"",""}},
    };

    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();

    return OldHeapDestroy(hHeap);
}

static BOOL(WINAPI* OldHeapFree)(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem) = HeapFree;
extern "C" __declspec(dllexport) BOOL WINAPI NewHeapFree(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem) {
    
    if (Heap.find(hHeap) == Heap.end())
        return OldHeapFree(hHeap, dwFlags, lpMem);
    else if (Heap.find(hHeap)->second == true)
        Heap.find(hHeap)->second = false;
    else {}


    char send_info[3][80];
    char time[80]; GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    //参数值
    sprintf_s(send_info[0], "%08X", hHeap);
    sprintf_s(send_info[1], "%08X", dwFlags);
    sprintf_s(send_info[2], "%08X", lpMem);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    json info = {
        { "type","HEAPFREE" },
        {"time",time},
        { "args",
        {"hHeap",send_info[0]},
        {"dwFlags",send_info[1]},
        {"lpMem",send_info[2]}}
    };
    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();

    return OldHeapFree(hHeap,dwFlags,lpMem);
}


static LPVOID (WINAPI* OldHeapAlloc)(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes) = HeapAlloc;
extern "C" __declspec(dllexport) LPVOID WINAPI NewHeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes) {
    if (Heap.find(hHeap) == Heap.end())        
        return OldHeapAlloc(hHeap, dwFlags, dwBytes);
    else Heap.find(hHeap)->second =true;

    if (freenumber != allocnumber) {
        printf(" heap reaptedly free!!\n");
        freenumber == allocnumber;
    }

    char send_info[3][80];
    char time[80]; 
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    //参数值
    sprintf_s(send_info[0], "%08X", hHeap);
    sprintf_s(send_info[1], "%08X", dwFlags);
    sprintf_s(send_info[2], "%08X", dwBytes);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    json info = {
        { "type","HEAPALLOC" },
        {"time",time},
        { "args",
        {"hHeap",send_info[0]},
        {"dwFlags",send_info[1]},
        {"lpMem",send_info[2]}}
    };
    std::ofstream file("information.json", std::ios::app);
    file << info << std::endl;
    file.close();

    return OldHeapAlloc(hHeap, dwFlags, dwBytes);
}



static LSTATUS(WINAPI* OldRegCreateKeyEx)(
    HKEY                        hKey,
    LPCWSTR                     lpSubKey,
    DWORD                       Reserved,
    LPWSTR                      lpClass,
    DWORD                       dwOptions,
    REGSAM                      samDesired,
    const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY                       phkResult,
    LPDWORD                     lpdwDisposition) = RegCreateKeyEx;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCreateKeyEx(
    HKEY                        hKey,
    LPCWSTR                     lpSubKey,
    DWORD                       Reserved,
    LPWSTR                      lpClass,
    DWORD                       dwOptions,
    REGSAM                      samDesired,
    const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY                       phkResult,
    LPDWORD                     lpdwDisposition
) {
    char temp[70];
    char send_info[9][80];
    // 参数值
    sprintf_s(send_info[0], "%08X", hKey);
    sprintf_s(send_info[1], "%08X", lpSubKey);
    sprintf_s(send_info[2], "%08X", Reserved);
    sprintf_s(send_info[3], "%08X", lpClass);
    sprintf_s(send_info[4], "%08X", dwOptions);
    sprintf_s(send_info[5], "%08X", samDesired);
    sprintf_s(send_info[6], "%08X", lpSecurityAttributes);
    sprintf_s(send_info[7], "%08X", phkResult);
    sprintf_s(send_info[8], "%08X", lpdwDisposition);

    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return OldRegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}


static LSTATUS(WINAPI* OldRegSetValueEx)(
    HKEY       hKey,
    LPCWSTR    lpValueName,
    DWORD      Reserved,
    DWORD      dwType,
    const BYTE* lpData,
    DWORD      cbData
    ) = RegSetValueEx;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegSetValueEx(
    HKEY       hKey,
    LPCWSTR    lpValueName,
    DWORD      Reserved,
    DWORD      dwType,
    const BYTE * lpData,
    DWORD      cbData)
{
    char sendInfo[5][80];
    // 参数值
    char time[80];
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    sprintf_s(sendInfo[0], "%08X", hKey);
    sprintf_s(sendInfo[1], "%08X",lpValueName);
    sprintf_s(sendInfo[2],"%08X", Reserved);
    sprintf_s(sendInfo[3], "%08X", dwType);
    //strcpy(sendInfo.argValue[4], (const char *)lpData);
    sprintf_s(sendInfo[4], "%08X", lpData);
    sprintf_s(sendInfo[5], "%08X", cbData);
   // memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return OldRegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

static LSTATUS(WINAPI* OldRegCloseKey)(HKEY hKey) = RegCloseKey;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegCloseKey(HKEY hKey)
{
    char time[80];
    char sendInfo[1][80];
    // 参数值
    sprintf_s(sendInfo[0], "%08X", hKey);
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return OldRegCloseKey(hKey);
}

static LSTATUS(WINAPI* OldRegOpenKeyEx)(
    HKEY    hKey,
    LPCWSTR lpSubKey,
    DWORD   ulOptions,
    REGSAM  samDesired,
    PHKEY   phkResult
    ) = RegOpenKeyEx;
extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegOpenKeyEx(
    HKEY    hKey,
    LPCWSTR lpSubKey,
    DWORD   ulOptions,
    REGSAM  samDesired,
    PHKEY   phkResult)
{
    char sendInfo[5][80];
    char time[80];
    // 参数值
    sprintf_s(sendInfo[0], "%08X", hKey);
    sprintf_s(sendInfo[1], "%08X", lpSubKey);
    sprintf_s(sendInfo[2], "%08X", ulOptions);
    sprintf_s(sendInfo[3], "%08X", samDesired);
    sprintf_s(sendInfo[4], "%08X", phkResult);
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return OldRegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

static LSTATUS(WINAPI* OldRegDeleteValue)(
    HKEY    hKey,
    LPCWSTR lpValueName
    ) = RegDeleteValue;

extern "C" __declspec(dllexport)LSTATUS WINAPI NewRegDeleteValue(
    HKEY    hKey,
    LPCWSTR lpValueName)
{
 
    char time[80];
    char sendInfo[2][80];
    sprintf_s(sendInfo[0], "%08X", hKey);
    sprintf_s(sendInfo[1], "%08X", lpValueName);
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return OldRegDeleteValue(hKey, lpValueName);
}


static SOCKET(WINAPI* Oldsocket)(
    int af,
    int type,
    int protocol
    ) = socket;
extern "C" __declspec(dllexport)SOCKET WINAPI Newsocket(int af, int type, int protocol) {
    char time[80];
    char sendInfo[3][80];
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    sprintf(sendInfo[0], "%08X", af);
    sprintf(sendInfo[1], "%08X", type);
    sprintf(sendInfo[2], "%08X", protocol);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return Oldsocket(af, type, protocol);
}

static int (WINAPI* Oldbind)(
    SOCKET         s,
    const sockaddr* name,
    int            namelen
    ) = bind;
extern "C" __declspec(dllexport)int WINAPI Newbind(SOCKET s, const sockaddr * name, int namelen) {
    char sendInfo[5][80];
    char time[80];
    sprintf(sendInfo[0], "%08X", s);
    sprintf(sendInfo[1], "%08X", name);
    sprintf(sendInfo[2], "%08X", namelen);
    struct sockaddr_in* sock = (struct sockaddr_in*)name;
    int port = ntohs(sock->sin_port);
    sprintf(sendInfo[3], "%s", inet_ntoa(sock->sin_addr));
    sprintf(sendInfo[4], "%d", port);
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return Oldbind(s, name, namelen);
}

static int (WINAPI* Oldsend)(
    SOCKET     s,
    const char* buf,
    int        len,
    int        flags
    ) = send;
extern "C" __declspec(dllexport)int WINAPI Newsend(SOCKET s, const char* buf, int len, int flags) {
    char time[80];
    char sendInfo[5][80];
    sprintf(sendInfo[0], "%08X", s);
    sprintf(sendInfo[1], "%08X", buf);
    sprintf(sendInfo[2], "%08X", len);
    sprintf(sendInfo[3], "%08X", flags);
    sprintf(sendInfo[4], "%s", buf);
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return Oldsend(s, buf, len, flags);
}

static int (WINAPI* Oldconnect)(
    SOCKET         s,
    const sockaddr* name,
    int            namelen
    ) = connect;
extern "C" __declspec(dllexport)int WINAPI Newconnect(SOCKET s, const sockaddr * name, int namelen) {
    //name->sa_data
    char time[80];
    char sendInfo[5][80];
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    sprintf(sendInfo[0], "%08X", s);
    sprintf(sendInfo[1], "%08X", name);
    sprintf(sendInfo[2], "%08X", namelen);
    struct sockaddr_in* sock = (struct sockaddr_in*)name;
    int port = ntohs(sock->sin_port);
    sprintf(sendInfo[3], "%s", inet_ntoa(sock->sin_addr));
    sprintf(sendInfo[4], "%d", port);
    //GetLocalTime(&(sendInfo.st));
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return Oldconnect(s, name, namelen);
}

static void* (__cdecl* Oldmemcpy)(void* _Dst, const void* _Src, size_t _Size) = memcpy;
extern "C" __declspec(dllexport)void* __cdecl Newmemcpy(void* _Dst, const void* _Src, size_t _Size) {
    printf("memcpy hooked\n");
    return Oldmemcpy(_Dst, _Src, _Size);
}
//void* __cdecl memcpy(void* _Dst, const void* _Src, size_t _Size)

//static void (WINAPI* OldRtlCopyMemory)(
//	void* Destination,
//	const void* Source,
//	size_t      Length	
//	) = RtlCopyMemory;


static int (WINAPI* Oldrecv)(
    SOCKET     s,
    char* buf,
    int        len,
    int        flags
    ) = recv;
extern "C" __declspec(dllexport)int WINAPI Newrecv(SOCKET s, char* buf, int len, int flags) {
    char time[80];
    char sendInfo[4][80];
    GetLocalTime(&st);
    sprintf_s(time, "%d-%d-%d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    sprintf(sendInfo[0], "%08X", s);
    sprintf(sendInfo[1], "%08X", buf);
    sprintf(sendInfo[2], "%08X", len);
    sprintf(sendInfo[3], "%08X", flags);
    
    //memcpy(lpBase, &sendInfo, sizeof(info));
    //ReleaseSemaphore(hSemaphore, 1, NULL);
    return Oldrecv(s, buf, len, flags);
}



BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    //LPWSTR Buf;
    //GetModuleFileName(0, Buf, 256);

   // strcmp("C:\\Users\\xp\\Desktop\\softwaresecurity\\InjectDll\\InnjectDll\\",Buf);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
        DetourAttach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);
        //DetourAttach(&(PVOID&)OldCreateFile, NewCreateFile);
        DetourAttach(&(PVOID&)OldHeapCreate, NewHeapCreate);
        DetourAttach(&(PVOID&)OldHeapDestroy, NewHeapDestroy);
        DetourAttach(&(PVOID&)OldHeapFree, NewHeapFree);
        DetourAttach(&(PVOID&)OldHeapAlloc, NewHeapAlloc);
        DetourAttach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);
        DetourAttach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);
        DetourAttach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
        DetourAttach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);
        DetourAttach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
        DetourAttach(&(PVOID&)Oldsocket, Newsocket);
        DetourAttach(&(PVOID&)Oldsend, Newsend);
        DetourAttach(&(PVOID&)Oldbind, Newbind);
        DetourAttach(&(PVOID&)Oldrecv, Newrecv);
        DetourAttach(&(PVOID&)Oldmemcpy, memcpy);
        DetourAttach(&(PVOID&)Oldconnect, Newconnect);

        DetourTransactionCommit();
    }
    break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
        DetourAttach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);
        //DetourAttach(&(PVOID&)OldCreateFile, NewCreateFile);
        DetourAttach(&(PVOID&)OldHeapCreate, NewHeapCreate);
        DetourAttach(&(PVOID&)OldHeapDestroy, NewHeapDestroy);
        DetourAttach(&(PVOID&)OldHeapFree, NewHeapFree);
        DetourAttach(&(PVOID&)OldHeapAlloc, NewHeapAlloc);
        DetourAttach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);
        DetourAttach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);
        DetourAttach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
        DetourAttach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);
        DetourAttach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
        DetourAttach(&(PVOID&)OldHeapAlloc, NewHeapAlloc);
        DetourAttach(&(PVOID&)OldHeapAlloc, NewHeapAlloc);
        DetourAttach(&(PVOID&)OldHeapAlloc, NewHeapAlloc);
        DetourAttach(&(PVOID&)Oldsocket, Newsocket);
        DetourAttach(&(PVOID&)Oldsend, Newsend);
        DetourAttach(&(PVOID&)Oldbind, Newbind);
        DetourAttach(&(PVOID&)Oldrecv, Newrecv);
        DetourAttach(&(PVOID&)Oldmemcpy, memcpy);
        DetourAttach(&(PVOID&)Oldconnect, Newconnect);

        DetourTransactionCommit();
        break;
    }
    return TRUE;
}

