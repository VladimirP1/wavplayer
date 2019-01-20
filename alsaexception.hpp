#pragma once
#include <exception>

class AlsaException : std::exception  {
public:

    AlsaException(char const* message, int error, bool isFatal) : message(message), error(error), fatal(isFatal) {}

    int getErrno() { return error; }

    bool isFatal() { return fatal; }

    virtual char const* what() const noexcept override { return message; }

private:
    char const* message;
    int error;
    bool fatal;
};