//
// Created by luna on 14/09/2022.
//

#include "ModuleWrapper.h"


ModuleWrapper::ModuleWrapper() {
    inner_handle = GetModuleHandleA(nullptr);
    auto proc_handl = GetCurrentProcess();
    GetModuleInformation(
            proc_handl,
            (HMODULE) inner_handle,
            &mod_info,
            sizeof(MODULEINFO)
    );
    std::cout << "Module base: " << mod_info.lpBaseOfDll << std::endl;

}

std::optional<char*> ModuleWrapper::scan(const char * sig) {
    MEMORY_BASIC_INFORMATION mbi{};
    for (char* current = reinterpret_cast<char *>(inner_handle); current < reinterpret_cast<char*>(inner_handle) + mod_info.SizeOfImage; current += mbi.RegionSize) {
        VirtualQuery(current,&mbi,sizeof(mbi));
        if (mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) {
            continue;
        }

        auto f = sig_scan(sig, current, mbi.RegionSize);
        if (f.has_value()) {
            return f.value();
            break;
        }
    }
    return {};
}

std::optional<char *> ModuleWrapper::sig_scan(const char* sig, char* start, intptr_t size) {
    std::string_view sig_str = sig;
    std::cout << "Scanning: " << (void*)start << " - " << (void*)(start + size) << std::endl;
    for (uint32_t i = 0 ; i < size; i++) {
        bool okay = true;
        int offset = 0;
        for (int j =0; j < sig_str.size(); j++) {
            auto char_at = sig_str[j];
            if (char_at == '?') {
                offset++;
                continue;
            }
            if (char_at == ' ') {continue;};
            int as_int;
            std::from_chars(sig_str.data() + j, sig_str.data() + j + 2,as_int, 16);
//            if (offset > 7) {
//                std::cout << "offset: " << std::dec << offset << "c = " << std::hex << (int)as_int << std::endl;
//            }
            if ((char)as_int != *(char*)((intptr_t)start + i + offset)) {
                okay = false;
                break;
            }
            offset++;
            j++;
        }
        if (okay) {
            std::cout << "pogging\n";
            return start + i;
        }
    }
    return {};

}

ModuleWrapper::ModuleWrapper(const char *module_name) {
    inner_handle = GetModuleHandleA(module_name);
    auto proc_handl = GetCurrentProcess();
    GetModuleInformation(
            proc_handl,
            (HMODULE) inner_handle,
            &mod_info,
            sizeof(MODULEINFO)
    );
    std::cout << "Module base: " << mod_info.lpBaseOfDll << std::endl;
}

char *ModuleWrapper::get_relative(char *instruction, uint32_t offset) {
    return reinterpret_cast<char *>((instruction - (char *) inner_handle) + offset);
}

char *ModuleWrapper::resolve_relative_ptr(char *instruction, uint32_t offset) {
    uint32_t act_offset;

    std::memcpy(&act_offset, (void*)(instruction + offset), 4);

    return (intptr_t)inner_handle + get_relative(instruction, act_offset + offset + 4);
}
