#pragma once

#include <exception>
#include <string>

class Exception : public std::exception {
    public:
        std::string _message;

        Exception(std::string message) noexcept:
            _message(message)
        {
        }

        virtual ~Exception() noexcept
        {
        }

        virtual const char* what() const noexcept
        {
            return _message.c_str();
        }
};

