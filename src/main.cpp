#include <iostream>
#include <cstdlib>

#include <nethost.h>
#include <coreclr_delegates.h>
#include <hostfxr.h>

#include <dlfcn.h>
#include <climits>
#include <cassert>

#define STR(s) s
#define CH(c) c
#define DIR_SEPARATOR '/'
#define MAX_PATH PATH_MAX

using string_t = std::basic_string<char_t>;

namespace {
    hostfxr_initialize_for_runtime_config_fn init_fncPointer;
    hostfxr_get_runtime_delegate_fn get_delegate_fncPointer;
    hostfxr_close_fn close_fncPointer;

    // forward declarations
    bool load_hostfxr();
    load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *assembly);
}

int main(int argc, char *argv[])
{
    // get executable directory
    char_t hostPath[MAX_PATH];
    auto resolved = realpath(argv[0], hostPath);
    assert(resolved != nullptr);

    string_t rootPath = hostPath;
    auto pos = rootPath.find_last_of(DIR_SEPARATOR);
    assert(pos != string_t::npos);
    rootPath = rootPath.substr(0, pos + 1);


    // load hostfxr and get exporting hosting funcs
    if(!load_hostfxr())
    {
        assert(false && "Failure: load_hostfxr()");
        return EXIT_FAILURE;
    }

    
    // init and start runtime
    const string_t configPath = rootPath + STR("DotNetLib.runtimeconfig.json");
    load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;
    load_assembly_and_get_function_pointer = get_dotnet_load_assembly(configPath.c_str());
    assert(load_assembly_and_get_function_pointer != nullptr && "Failure: get_dotnet_load_assembly()");

    // load
    const string_t dotnetlibPath = rootPath + STR("DotNetLib.dll");
    const char_t *dotnetType = STR("DotNetLib.Lib, DotNetLib");
    const char_t *dotnetTypeMethod = STR("Hello");

    std::cout << dotnetlibPath << "\n";
    //return 0;
    // <SnippetLoadAndGet>
    //fnc pointer to managed delegate
    component_entry_point_fn hello = nullptr;
    int rc = load_assembly_and_get_function_pointer(
        dotnetlibPath.c_str(),
        dotnetType,
        dotnetTypeMethod,
        nullptr,    // delegate_type_name
        nullptr,
        (void**)&hello
    );
    // </SnippetLoadAndGet>
    assert(rc == 0 && hello != nullptr && "Failure: load_assembly_and_get_function_pointer()");

    // run code

    struct lib_args
    {
        const char_t *message;
        int number;
    };

    for(int i = 0; i < 3; i++)
    {
        // <SnippetCallManaged>
        lib_args args
        {
            STR("from host!"),
            i
        };
        hello(&args, sizeof(args));
    }

    // Function pointer to managed delegate with non-default signature
    typedef void (CORECLR_DELEGATE_CALLTYPE *custom_entry_point_fn)(lib_args args);
    custom_entry_point_fn custom = nullptr;
    rc = load_assembly_and_get_function_pointer(
        dotnetlibPath.c_str(),
        dotnetType,
        STR("CustomEntryPoint") /*method_name*/,
        STR("DotNetLib.Lib+CustomEntryPointDelegate, DotNetLib") /*delegate_type_name*/,
        nullptr,
        (void**)&custom);
    assert(rc == 0 && custom != nullptr && "Failure: load_assembly_and_get_function_pointer()");

    lib_args args
    {
        STR("from host!"),
        -1
    };
    custom(args);

    return EXIT_SUCCESS;
}

//Function used to load and activate .NET Core

namespace
{
    // Forward declarations
    void *load_library(const char_t *);
    void *get_export(void *, const char *);

    void *load_library(const char_t* path)
    {
        void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
        assert(h != nullptr);
        return h;
    }

    void *get_export(void *h, const char *name)
    {
        void *f = dlsym(h, name);
        assert(f != nullptr);
        return f;
    }

    // Using the nethost library, discover the location of hostfxr and get exports
    bool load_hostfxr()
    {
        // Pre-allocate a large buffer for the path to hostfxr
        char_t buffer[MAX_PATH];
        size_t bufferSize = sizeof(buffer) / sizeof(char_t);
        int rc = get_hostfxr_path(buffer, &bufferSize, nullptr);
        if(rc != 0)
        {
            return false;
        }

        // load hostfxr and get desired exports
        void *lib = load_library(buffer);
        init_fncPointer = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
        get_delegate_fncPointer = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
        close_fncPointer = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

        return (init_fncPointer && get_delegate_fncPointer && close_fncPointer);
    }

    // Load and initialize .NET Core and get desired function pointer for scenario
    load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *config_path)
    {
        // load .Net Core
        void *load_assembly_and_get_function_pointer = nullptr;
        hostfxr_handle cxt = nullptr;
        int rc = init_fncPointer(config_path, nullptr, &cxt);
        if(rc != 0 || cxt == nullptr)
        {
            std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
            close_fncPointer(cxt);
            return nullptr;
        }
        //get the load assembly function pointer
        rc = get_delegate_fncPointer(
            cxt,
            hdt_load_assembly_and_get_function_pointer,
            &load_assembly_and_get_function_pointer
        );
        if(rc != 0 || load_assembly_and_get_function_pointer == nullptr)
        {
            std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;
        }

        close_fncPointer(cxt);
        return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
    }
}