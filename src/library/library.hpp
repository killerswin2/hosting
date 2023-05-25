#pragma once

#include <iostream>
#include <cstdlib>
#include <filesystem>

#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

#include <dlfcn.h>
#include <climits>
#include <cassert>

class Library
{
private:
    std::filesystem::path _path;
#ifdef _WIN32
#else
    void* _library;
#endif
public:
    Library(std::filesystem::path& libraryPath): _path{std::move(libraryPath)} {}
    void load_library();

    template<typename t1>
    t1 get_export(const char * name)
    {
        void *f = dlsym(_library, name);
        assert(f != nullptr);
        return reinterpret_cast<t1>(f);
    }
};