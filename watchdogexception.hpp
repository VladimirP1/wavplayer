#pragma once
#include <exception>

class WatchdogException : std::exception  {
public:

    WatchdogException(char const* message, int error) : message(message), error(error) {}

    int getErrno() { return error; }

    virtual char const* what() const noexcept override { return message; }

private:
    char const* message;
    int error;
};