//
// Created by luna on 14/09/2022.
//

#ifndef GODOT_KEY_EXTRACT_MODULEWRAPPER_H
#define GODOT_KEY_EXTRACT_MODULEWRAPPER_H

#include "library.h"
#include <psapi.h>


class ModuleWrapper {

public:
    HANDLE inner_handle;
    ModuleWrapper();
    ModuleWrapper(const char * module_name);
    std::optional<char*> scan(const char *sig);
    char* get_relative(char* instruction, uint32_t offset);
    char* resolve_relative_ptr(char* instruction, uint32_t offset);
private:
    std::optional<char*>sig_scan(const char* sig, char* start, intptr_t size);
    MODULEINFO mod_info;
};


#endif //GODOT_KEY_EXTRACT_MODULEWRAPPER_H
