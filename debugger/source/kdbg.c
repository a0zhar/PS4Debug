#include "../include/kdbg.h"

// A custom malloc wrapper, that will ensure that the allocated memory is prefaulted.
void *pfmalloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) return NULL;

    // Prefault() logic directly used here
    for (uint64_t i = 0; i < size; i++) {
        volatile uint8_t c;
        (void)c;

        c = ((char *)p)[i];
    }

    return p;
}

void hexdump(void *data, size_t size) {
    unsigned char *p = (unsigned char *)data;
    for (int i = 0; i < size; i++) {
        uprintf("%02X ", *p++);
        if (!(i % 16) && i != 0) {
            uprintf("\n");
        }
    }
    uprintf("\n");
}

// custom syscall 107
int sys_proc_list(struct proc_list_entry *procs, uint64_t *num) {
    return syscall(107, procs, num);
}

// custom syscall 108
int sys_proc_rw(uint64_t pid, uint64_t address, void *data, uint64_t length, uint64_t write) {
    return syscall(108, pid, address, data, length, write);
}

// custom syscall 109
int sys_proc_cmd(uint64_t pid, uint64_t cmd, void *data) {
    return syscall(109, pid, cmd, data);
}

// custom syscall 110
int sys_kern_base(uint64_t *kbase) {
    return syscall(110, kbase);
}

// custom syscall 111
int sys_kern_rw(uint64_t address, void *data, uint64_t length, uint64_t write) {
    return syscall(111, address, data, length, write);
}

// custom syscall 112
int sys_console_cmd(uint64_t cmd, void *data) {
    return syscall(112, cmd, data);
}

int uprintf(const char *fmt, ...) {
    char buffer[256] = { 0 };
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf_(buffer, sizeof(buffer) - 1, fmt, args);
    va_end(args);
    sys_console_cmd(SYS_CONSOLE_CMD_PRINT, buffer);
    return len;
}