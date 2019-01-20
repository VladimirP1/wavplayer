#include "wdt.hh"

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/watchdog.h>

Watchdog::Watchdog(char const* filename) {
    fd = open(filename, O_WRONLY);
    if (fd < 0) {
        throw WatchdogException("Could not open watchdog", errno);
    }
}

Watchdog::~Watchdog() {
    close(fd);
}

void Watchdog::kick() {
    int err = write(fd, "k", 1);

    if(err < 0) {
        throw WatchdogException("Could not kick watchdog", errno);
    }
}

void Watchdog::magic() {
    int err = write(fd, "V", 1);

    if(err < 0) {
        throw WatchdogException("Could not magic watchdog", errno);
    }
}

int Watchdog::setTimeout(int timeout) {
	int ret;

    ret = ioctl(fd, WDIOC_SETTIMEOUT, &timeout);

    if (ret < 0) {
        throw WatchdogException("Could not set timeout", ret);
    }

    return timeout;
}
