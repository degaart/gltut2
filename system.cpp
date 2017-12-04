#include "system.h"
#include "exception.h"

#include <unistd.h>
#include <mach-o/dyld.h>
#include <libgen.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include <system_error>

using std::string;
using std::vector;

string sys::dirname(const string& path)
{
    char* buffer = strdup(path.c_str());
    char* dname = ::dirname(buffer);

    string result(dname);
    free(buffer);

    return result;
}

string sys::exepath(int argc, const char* const* argv)
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

    string result(buffer);
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

vector<unsigned char> sys::readfile(const string& filename)
{
    vector<unsigned char> result;

    int fd = open(filename.c_str(), O_RDONLY);
    if(fd == -1) {
        throw errno_exception();
    }

    while(true) {
        char buffer[512];
        ssize_t ret;
        do {
            ret = read(fd, buffer, sizeof(buffer));
        } while(ret == -1 && errno == EINTR);

        if(ret == 0)
            break;
        else if(ret == -1)
            throw errno_exception();

        result.insert(result.end(), buffer, buffer + ret);
    }

    return result;
}

std::exception sys::errno_exception()
{
    return errno_exception(errno);
}

std::exception sys::errno_exception(int code)
{
    return std::system_error(code, std::system_category());
}


