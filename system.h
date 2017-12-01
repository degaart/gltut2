#pragma once

#include <string>

namespace sys {
    std::string dirname(const std::string& path);
    std::string exepath(int argc, const char* const* argv);
    bool exists(const std::string& path);
}


