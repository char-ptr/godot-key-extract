#include "library.h"
#include "ModuleWrapper.h"
// signature offset
std::pair<const char *,int16_t>function_sig[] = {
        {"48 8D 05 ? ? ? ? 41 8A 04 04", 0},
        {"48 8D 05 ? ? ? ? 0F B6 ? 03", 0},
        {"4C 8D 05 ? ? ? ? 0F 1F 40 00", 0}
};



void main_thread() {
    AllocConsole();
    FILE *f;
    freopen_s(&f, "CONOUT$", "w", (FILE *) stdout);
    freopen_s(&f, "CONIN$", "r", (FILE *) stdin);

    auto module = ModuleWrapper();

    char* lea_instr = nullptr;
    uint32_t used_offset = 0;
    for (auto sig : function_sig) {
        used_offset++;
        auto result = module.scan(sig.first);
        if (result.has_value()) {
            lea_instr = result.value() + sig.second;
            break;
        }
    }
    if (lea_instr != nullptr) {
        std::cout << "f\n";
        std::cout << "Found: " << (void*)lea_instr << " (" << used_offset << ")" << std::endl;
        auto resolved = module.resolve_relative_ptr(lea_instr, 3);
        if (resolved == nullptr) {
            std::cout << "unable to resolve relative offset" << std::endl;
        }
        else {
            std::cout << "Resolved: " << (void*)resolved << std::endl;
            uint8_t secretKey[32];
            memcpy(secretKey, resolved, 32);

            std::cout << "Secret key: ";
            for (auto i : secretKey) {
                std::cout << std::hex << (int)i;
            }
            std::cout << std::endl;
        }

    } else {
        std::cout << "Not found" << std::endl;
    }


    std::cout << "ok you can close me without killing main process now";

    fclose(f);
    FreeConsole();
}


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(GetModuleHandleA(nullptr));
            std::thread mt(main_thread);
            mt.detach();
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}