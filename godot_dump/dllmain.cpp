// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <thread>
#include <iostream>

char* ScanBasic(char* pattern, char* mask, char* begin, intptr_t size)
{
    intptr_t patternLen = strlen(mask);

    for (int i = 0; i < size; i++)
    {
        bool found = true;
        for (int j = 0; j < patternLen; j++)
        {
            if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return (begin + i);
        }
    }
    return nullptr;
}



char* ScanInternal(char* pattern,char * mask, char* begin, intptr_t size)
{
    char* match{ nullptr };
    MEMORY_BASIC_INFORMATION mbi{};

    for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
    {
        if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;

        match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

        if (match != nullptr)
        {
            break;
        }
    }
    return match;
}






void main_thread() {
    AllocConsole();
    freopen_s((FILE**)stdout,"CONOUT$", "w", (FILE*)stdout);
    freopen_s((FILE**)stdin,"CONIN$", "w", (FILE*)stdin);

    auto pog1 = GetModuleHandleA(nullptr);
    
    const char* load_byte_code_sig = "\x4C\x8D\x05\xCC\xCC\xCC\xCC\x0F\x1F\x40\x00";

    auto pog = ScanInternal((char*)load_byte_code_sig, (char*)"xxx????xxxx", (char *)pog1, 0xfffffff);

    uint32_t offset;

    std::memcpy(&offset, (void*)(pog + 3), 4);

    void* next = (void*)((pog + 7) - (char *)pog1);

    auto parta = (char*)next + offset;
    void* location = reinterpret_cast<char*>(pog1) + (uint32_t)parta;
    
    std::cout << "key loc @ " << location << std::endl;

    uint8_t secretKey[32];

    std::memcpy(&secretKey, location, 32);

    std::cout << "key = ";
    for (int i = 0; i < 32; i++) {
        std::cout << std::hex << (int) secretKey[i];
    }
    std::cout << std::dec << ";" << std::endl;
    std::cin.get();

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        std::thread pog(main_thread);
        pog.detach();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

