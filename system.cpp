#include "system.h"
#include "exception.h"

#include <unistd.h>
#include <mach-o/dyld.h>
#include <libgen.h>
#include <sys/stat.h>
#include <stdlib.h>

std::string sys::dirname(const std::string& path)
{
    char* buffer = strdup(path.c_str());
    char* dname = ::dirname(buffer);

    std::string result(dname);
    free(buffer);

    return result;
}

std::string sys::exepath(int argc, const char* const* argv)
{
#ifdef __APPLE__
    uint32_t bufSize = 512;
    char* buffer = static_cast<char*>(malloc(bufSize));
    int ret = _NSGetExecutablePath(buffer, &bufSize);
    if(ret != 0) {
        buffer = static_cast<char*>(realloc(buffer, bufSize));
        ret = _NSGetExecutablePath(buffer, &bufSize);
        if(ret != 0) {
            throw Exception("Failed to get executable path");
        }
    }

    std::string result(buffer);
    free(buffer);
    return result;
#else
#error "Not implemented yet"
#endif
}

bool sys::exists(const std::string& path)
{
    struct stat st;
    int ret = ::stat(path.c_str(), &st);
    return ret == 0;
}

