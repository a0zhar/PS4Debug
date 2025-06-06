#ifndef _KDEBUGGER_H
#define _KDEBUGGER_H

#include <ps4.h>
#include <stdarg.h>

// Used in the wrapper for the custom PS4 syscall (112)
#define SYS_CONSOLE_CMD_REBOOT    1
#define SYS_CONSOLE_CMD_PRINT     2
#define SYS_CONSOLE_CMD_JAILBREAK 3

// Used in the wrapper for the custom PS4 syscall (109)
#define SYS_PROC_CMD_ALLOC        1
#define SYS_PROC_CMD_FREE         2
#define SYS_PROC_CMD_PROTECT      3
#define SYS_PROC_CMD_CALL         5
#define SYS_PROC_ALLOC            1
#define SYS_PROC_FREE             2
#define SYS_PROC_PROTECT          3
#define SYS_PROC_VM_MAP           4
#define SYS_PROC_INSTALL          5
#define SYS_PROC_CALL             6
#define SYS_PROC_ELF              7
#define SYS_PROC_INFO             8
#define SYS_PROC_THRINFO          9

// custom syscall 107
struct proc_list_entry {
    char p_comm[32];
    int pid;
}  __attribute__((packed));

struct sys_proc_alloc_args {
    uint64_t address;
    uint64_t length;
} __attribute__((packed));

struct sys_proc_free_args {
    uint64_t address;
    uint64_t length;
} __attribute__((packed));

struct sys_proc_protect_args {
    uint64_t address;
    uint64_t length;
    uint64_t prot;
} __attribute__((packed));

struct sys_proc_vm_map_args {
    struct proc_vm_map_entry *maps;
    uint64_t num;
} __attribute__((packed));

struct sys_proc_install_args {
    uint64_t stubentryaddr;
} __attribute__((packed));

struct sys_proc_call_args {
    uint32_t pid;
    uint64_t rpcstub;
    uint64_t rax;
    uint64_t rip;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t r8;
    uint64_t r9;
} __attribute__((packed));

struct sys_proc_elf_args {
    void *elf;
} __attribute__((packed));

struct sys_proc_info_args {
    int pid;
    char name[40];
    char path[64];
    char titleid[16];
    char contentid[64];
} __attribute__((packed));

struct sys_proc_thrinfo_args {
    uint32_t lwpid;
    uint32_t priority;
    char name[32];
} __attribute__((packed));

void prefault(void *address, size_t size);
void *pfmalloc(size_t size);
void hexdump(void *data, size_t size);
int uprintf(const char*fmt,...);
int sys_proc_cmd(uint64_t pid, uint64_t cmd, void *data);
int sys_kern_base(uint64_t *kbase);
int sys_kern_rw(uint64_t address, void *data, uint64_t length, uint64_t write);
int sys_proc_list(struct proc_list_entry *procs, uint64_t *num);
int sys_proc_rw(uint64_t pid, uint64_t address, void *data, uint64_t length, uint64_t write);
int sys_console_cmd(uint64_t cmd, void *data);
#endif
