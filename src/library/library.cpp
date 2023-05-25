#include "library.hpp"


void Library::load_library()
{
    if(std::filesystem::exists(_path))
    {
        void *h = dlopen(_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        assert(h != nullptr);
        _library = h;
    } 
}