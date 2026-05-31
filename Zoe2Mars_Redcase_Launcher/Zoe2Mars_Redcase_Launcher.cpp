#include <Windows.h>
#include <iostream>
#include "detours/detours.h"
int main()
{
    STARTUPINFOA startInfo{0};
    PROCESS_INFORMATION processInfo{ 0 };
    std::string DllName("Zoe2Mars_Redcase64.dll");
    DetourCreateProcessWithDllExA("Zoe2Mars.exe", nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, &startInfo, &processInfo, DllName.c_str(), nullptr);
    return 0;
}
