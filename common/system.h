#pragma once

#include <string>
#include <vector>
#include <exception>

namespace sys {
    std::string dirname(const std::string& path);
    std::string exepath(int argc, const char* const* argv);
    bool exists(const std::string& path);
    std::vector<unsigned char> readfile(const std::string& filename);
    std::exception errno_exception();
    std::exception errno_exception(int code);
}


