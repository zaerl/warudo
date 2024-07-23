#include "queue.h"

#include <unistd.h>

#ifdef __linux__
#include <sys/epoll.h>
#else
#include <sys/event.h>
#endif

WRD_API wrd_code wrd_start_queue(warudo *config) {
    // Start the event queue.
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);

    wrd_log_info(*config, u8"Cores: %l\n", num_cores);

    return WRD_OK;
}
