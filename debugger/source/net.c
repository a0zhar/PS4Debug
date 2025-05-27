#include "../include/net.h"


// Clears all bits in the fd_set
void FD_ZERO(fd_set *set) {
    if (!set) return;
    size_t count = sizeof(fd_set) / sizeof(fd_mask);
    for (size_t i = 0; i < count; i++) {
        set->fds_bits[i] = 0;
    }
}

// Sets bit corresponding to descriptor d
void FD_SET(int d, fd_set *set) {
    if (!set || d < 0 || d >= FD_SETSIZE)
        return;
    set->fds_bits[d / (8 * sizeof(long))] |= (1UL << (d % (8 * sizeof(long))));
}

// Clears bit corresponding to descriptor d
void FD_CLR(int d, fd_set *set) {
    if (!set || d < 0 || d >= FD_SETSIZE)
        return;
    set->fds_bits[d / (8 * sizeof(long))] &= ~(1UL << (d % (8 * sizeof(long))));
}

// Checks if bit is set for descriptor d
int FD_ISSET(int d, fd_set *set) {
    if (!set || d < 0 || d >= FD_SETSIZE)
        return 0;

    return (set->fds_bits[d / (8 * sizeof(long))] & (1UL << (d % (8 * sizeof(long))))) != 0;
}

int net_select(int fd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    return syscall(93, fd, readfds, writefds, exceptfds, timeout);
}

int net_send_data(int fd, void *data, int length) {
    int left = length;
    int offset = 0;
    int sent = 0;

    errno = NULL;

    while (left > 0) {
        if (left > NET_MAX_LENGTH) {
            sent = write(fd, data + offset, NET_MAX_LENGTH);
        }
        else {
            sent = write(fd, data + offset, left);
        }

        if (sent <= 0) {
            if (errno && errno != EWOULDBLOCK) {
                return sent;
            }
        }
        else {
            offset += sent;
            left -= sent;
        }
    }

    return offset;
}

int net_recv_data(int fd, void *data, int length, int force) {
    int left = length;
    int offset = 0;
    int recv = 0;

    errno = NULL;

    while (left > 0) {
        if (left > NET_MAX_LENGTH) {
            recv = read(fd, data + offset, NET_MAX_LENGTH);
        }
        else {
            recv = read(fd, data + offset, left);
        }

        if (recv <= 0) {
            if (force) {
                if (errno && errno != EWOULDBLOCK) {
                    return recv;
                }
            }
            else {
                return offset;
            }
        }
        else {
            offset += recv;
            left -= recv;
        }
    }

    return offset;
}

int net_send_status(int fd, uint32_t status) {
    uint32_t d = status;

    return net_send_data(fd, &d, sizeof(uint32_t));
}
