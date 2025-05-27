#include "../include/ptrace.h"

// Wrapper for the PS4 Syscall (26) that coresponds to ptrace
// @param req: Coresponds to rdi
// @param pid: Coresponds to rsi
// @param addr: Coresponds to rdx
// @param data: Coresponds to r10
int ptrace(int req, int pid, void *addr, int data) {
    errno = NULL;
    // UNUSED for now:
    // int r = syscall(26, req, pid, addr, data);
    // uprintf("ptrace(req %i, pid %i, addr 0x%llX, data 0x%X) = %i (errno %i)", req, pid, addr, data, r, errno);
    return syscall(26, req, pid, addr, data);
}

int wait4(int wpid, int *status, int options, void *rusage) {
    return syscall(7, wpid, status, options, rusage);
}
