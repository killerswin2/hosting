#include <iostream>
#include <cstdlib>
#include <filesystem>

#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

#include <dlfcn.h>
#include <climits>
#include <cassert>

#include "library/library.hpp"
#include "binding/binding.hpp"
#include "nethostfxr/nethostfxr.hpp"

using string_t = std::basic_string<char_t>;



int main(int argc, char *argv[])
{
    // get executable directory
    string_t rootPath = "/Users/ellisnielsen/git/hosting/cs_code/";
    std::filesystem::path path{rootPath};

    string_t assemblyName{"DotNetLib"};
    string_t className{"Lib"};

    Nethostfxr nethost{assemblyName, className, path};
    // init and start runtime

    auto hello = nethost.get_function_pointer<void (CORECLR_DELEGATE_CALLTYPE *)(int args)>("doMath");

    int number = 20;
    hello(number);

    return EXIT_SUCCESS;
}

//Function used to load and activate .NET Core
