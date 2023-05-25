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


#include <string>
#include <filesystem>
#include <functional>
#include <type_traits>

using string_t = std::basic_string<char_t>;

class Bindings
{
private:
    string_t _assemblyName;

    // used for custom delegate types
    string_t _assemblyStaticClassName;
    
    std::filesystem::path _assemblyPath;
    std::function<int(
        const char_t *assembly_path,    
        const char_t *type_name,   
        const char_t *method_name,
        const char_t *delegate_type_name,
        void *reserved,           
        void **delegate
    )> _load_assembly_and_get_function_pointer;

public:
    Bindings() {}
    Bindings(string_t& assemblyName, string_t assemblyStaticClassName, std::filesystem::path& assemblyPath, load_assembly_and_get_function_pointer_fn functionPointer);
    template<typename t1>
    t1 get_function_pointer_from_assembly(const string_t & methodName);
};

Bindings::Bindings(string_t& assemblyName, string_t assemblyStaticClassName, std::filesystem::path& assemblyPath, load_assembly_and_get_function_pointer_fn functionPointer) : _assemblyName{std::move(assemblyName)}, _assemblyStaticClassName{std::move(assemblyStaticClassName)}
{
    _assemblyPath.clear();
    // check that the path exists
    assemblyPath.append(_assemblyName+".dll");
    if(std::filesystem::exists(assemblyPath))
    {
        _assemblyPath = std::filesystem::canonical(assemblyPath);
        std::cout << "Path: " << _assemblyPath << "\n";
    }


    if(_assemblyPath.empty())
    {
        std::cout << "Path is empty\n";
    }

    _load_assembly_and_get_function_pointer = std::move(functionPointer);
}

//@TODO: make this cached.
template<typename t1>
t1 Bindings::get_function_pointer_from_assembly(const string_t & methodName)
{

    const string_t dotnetType = _assemblyName + "." + _assemblyStaticClassName + ", " + _assemblyName;
    const string_t delegateTypeName = _assemblyName + "." + _assemblyStaticClassName + "+" + "doMathDelegate" +", " + _assemblyName;

    t1 custom = nullptr;
    int rc = _load_assembly_and_get_function_pointer(
        _assemblyPath.c_str(),
        dotnetType.c_str(),
        methodName.c_str(), /*method_name*/
        delegateTypeName.c_str(),  /*delegate_type_name*/
        nullptr,
        (void**)&custom
    );
    
    if(rc != 0)
    {
        std::cout << "Error code: " << rc << "\n";
    }
    assert(rc == 0 && custom != nullptr && "Failure: load_assembly_and_get_function_pointer()");
    return custom;
}
