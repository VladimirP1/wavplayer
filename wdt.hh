#pragma once
#include "watchdogexception.hpp"

class Watchdog {
public:
    Watchdog(char const* filename);
    ~Watchdog();
    void kick();
    void magic();
    int setTimeout(int timeout);
private:
    int fd;
};