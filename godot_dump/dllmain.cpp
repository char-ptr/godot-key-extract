// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <thread>
#include <iostream>
#include <charconv>
#include <algorithm>

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

void* find_rel_addr(char* nextins,uint32_t offset) {
    char* mod = (char*)GetModuleHandleA(nullptr);
    auto parta = (nextins - mod) + offset;
    return mod + (uint32_t)parta;
}
void* find_rel_addr_lea(char* nextins, char* offset) {
    uint32_t act_offset;

    std::memcpy(&act_offset, (void*)(offset + 3), 4);

    return find_rel_addr(nextins, act_offset);
}

void main_thread() {
    AllocConsole();
    FILE* f;
    freopen_s(&f,"CONOUT$", "w", (FILE*)stdout);
    freopen_s(&f,"CONIN$", "r", (FILE*)stdin);


    auto pog1 = GetModuleHandleA(nullptr);

    const char* version_sig = "\x48\x8B\x4C\x24\x00\x48\x85\xC9\x74\x15\x8B\xC3\xF0\x0F\xC1\x41\x00\x83\xF8\x01\x75\x09\x0F\xB6\xD0\xE8\x00\x00\x00\x00\x90\x48\x8D\x15\x00\x00\x00\x00\x48\x8D\x4C\x24\x00\xE8\x00\x00\x00\x00\x90\x4C\x8D\x44\x24\x00\x48\x8B\xD7\x48\x8B\xC8\xE8\x00\x00\x00\x00\x90\x48\x8B\x4C\x24\x00\x48\x85\xC9\x74\x15\x8B\xC3\xF0\x0F\xC1\x41\x00\x83\xF8\x01\x75\x09\x0F\xB6\xD0\xE8\x00\x00\x00\x00\x90"
        ;
    const char* version_sig_mask = "xxxx?xxxxxxxxxxx?xxxxxxxxx????xxxx????xxxx?x????xxxxx?xxxxxxx????xxxxx?xxxxxxxxxxx?xxxxxxxxx????x";


    auto version_somewhere = ScanInternal((char*)version_sig, (char*)version_sig_mask, (char*)pog1, 0xfffffff);

    auto version_lea = version_somewhere + 31;


    auto version_addr = (char*)find_rel_addr_lea(version_lea + 7, version_lea);

    std::cout << "game version = " << version_addr << std::endl;


    uint8_t secretKey[32];

    auto vera = std::string(version_addr);
    std::remove(vera.begin(), vera.end(), '.');
    int ver = 0;
    auto version_n = std::from_chars(vera.data(), vera.data() + vera.size(), ver);

    //std::cout << "v = " << ver << "; e = " << vera << std::endl;

    
    if (ver > 344 || (ver < 100 && ver > 34)) {//if (ver.find("3.6") != std::string::npos) { // best hope, i cba to check others rn

        const char* lea_sig = "\x48\x8D\x05\x00\x00\x00\x00\x0F\xB6\x00\x03";

        auto lea = ScanInternal((char*)lea_sig, (char*)"xxx????xx?x", (char*)pog1, 0xfffffff);

        auto key_addr = (char*)find_rel_addr_lea(lea + 7, lea);

        std::memcpy(&secretKey, key_addr, 32);

    } else if (ver <= 344) {
        std::cout << "using 344\n";

        const char* lea_sig = "\x4C\x8B\xFF\x4C\x8D\x05\x00\x00\x00\x00";

        auto lea = ScanInternal((char*)lea_sig, (char*)"xxxxxx????", (char*)pog1, 0xfffffff) + 3;

        auto key_addr = (char*)find_rel_addr_lea(lea + 7, lea);

        std::memcpy(&secretKey, key_addr, 32);
    }

    std::cout << "key = ";
    for (int i = 0; i < 32; i++) {
        std::cout << std::hex << (int) secretKey[i];
    }
    std::cout << std::dec << ";" << std::endl;
    std::cout << "ok you can close me without killing main process now";

    fclose(f);
    FreeConsole();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(GetModuleHandleA(nullptr));
        std::thread mt(main_thread);
        //main_thread();
        mt.detach();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

