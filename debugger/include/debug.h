#ifndef _DEBUG_H
#define _DEBUG_H

#include <ps4.h>
#include "protocol.h"
#include "net.h"
#include "ptrace.h"

#define DEBUG_INTERRUPT_PACKET_SIZE 0x4A0
#define DEBUG_PORT 755

#define	DBREG_DR7_DISABLE       0x00
#define	DBREG_DR7_LOCAL_ENABLE  0x01
#define	DBREG_DR7_GLOBAL_ENABLE 0x02

#define	DBREG_DR7_LEN_1   0x00 // (1 Byte) Length
#define	DBREG_DR7_LEN_2   0x01 // (1 Byte) Length
#define	DBREG_DR7_LEN_4   0x03 // (1 Byte) Length
#define	DBREG_DR7_LEN_8   0x02 // (1 Byte) Length

#define	DBREG_DR7_EXEC    0x00 // break on execute      
#define	DBREG_DR7_WRONLY  0x01 // break on write        
#define	DBREG_DR7_RDWR    0x03 // break on read or write

#define	DBREG_DR7_GD      0x2000

#define	DBREG_DR7_SET(i, len, access, enable) ((uint64_t)((len) << 2 | (access)) << ((i) * 4 + 16) | (enable) << (i) * 2)
#define	DBREG_DR7_MASK(i)       ((uint64_t)(0xf) << ((i) * 4 + 16) | 0x3 << (i) * 2)
#define	DBREG_DR7_ENABLED(d, i)	(((d) & 0x3 << (i) * 2) != 0)
#define	DBREG_DR7_ACCESS(d, i)	((d) >> ((i) * 4 + 16) & 0x3)
#define	DBREG_DR7_LEN(d, i)	    ((d) >> ((i) * 4 + 18) & 0x3)
#define	DBREG_DRX(d,x)          ((d)->dr[(x)]) // reference dr0 - dr7 by register number

// Represents the general-purpose CPU register state 
struct __reg64 {
    uint64_t r_r15;    // Callee-saved register (used for general-purpose)
    uint64_t r_r14;    // Callee-saved
    uint64_t r_r13;    // Callee-saved
    uint64_t r_r12;    // Callee-saved
    uint64_t r_r11;    // Caller-saved
    uint64_t r_r10;    // Caller-saved
    uint64_t r_r9;     // Caller-saved
    uint64_t r_r8;     // Caller-saved
    uint64_t r_rdi;    // First argument to functions (per x86-64 calling convention)
    uint64_t r_rsi;    // Second argument
    uint64_t r_rbp;    // Base pointer (used for stack frame)
    uint64_t r_rbx;    // Callee-saved
    uint64_t r_rdx;    // Third argument / return value register
    uint64_t r_rcx;    // Fourth argument / loop counter
    uint64_t r_rax;    // Primary return value register
    uint32_t r_trapno; // Trap number (indicates reason for interrupt/trap)
    uint16_t r_fs;     // FS segment register (used for thread-local storage)
    uint16_t r_gs;     // GS segment register (may be used for TLS or kernel structures)
    uint32_t r_err;    // Error code pushed by CPU on exceptions
    uint16_t r_es;     // Extra segment (rarely used in modern software)
    uint16_t r_ds;     // Data segment
    uint64_t r_rip;    // Instruction pointer (next instruction to execute)
    uint64_t r_cs;     // Code segment selector
    uint64_t r_rflags; // CPU flags
    uint64_t r_rsp;    // Stack pointer
    uint64_t r_ss;     // Stack segment
};
// Contents of each x87 floating point accumulator
struct fpacc87 {uint8_t fp_bytes[10];};

// Contents of each SSE extended accumulator
struct xmmacc {uint8_t xmm_bytes[16];};

// Contents of the upper 16 bytes of each AVX extended accumulator
struct ymmacc {uint8_t ymm_bytes[16];};

// Contains state for x87 FPU and SSE control/status registers:
// this is the core part of the FXSAVE format
struct envxmm {
    uint16_t en_cw;         // control word (16bits)
    uint16_t en_sw;         // status word (16bits)
    uint8_t en_tw;          // tag word (8bits)
    uint8_t en_zero;        // ...
    uint16_t en_opcode;     // opcode last executed (11 bits )
    uint64_t en_rip;        // floating point instruction pointer
    uint64_t en_rdp;        // floating operand pointer
    uint32_t en_mxcsr;      // SSE sontorol/status register
    uint32_t en_mxcsr_mask; // valid bits in mxcsr
};

// Holds the full state of x87 and SSE registers. 
// This is used for saving/restoring FPU state in context switches or breakpoints.
struct savefpu {
    struct envxmm sv_env;
    struct {
        struct fpacc87 fp_acc;
        uint8_t fp_pad[6]; // padding
    } sv_fp[8];
    struct xmmacc sv_xmm[16];
    uint8_t sv_pad[96];

} __attribute__((aligned(16)));

// Header used in the XSAVE area that identifies which extended 
// states are present (AVX, MPX, etc.)
struct xstate_hdr {
    uint64_t xstate_bv;       // Bit vector of which xstates are saved
    uint8_t xstate_rsrv0[16]; // Reserved space
    uint8_t xstate_rsrv[40];  // Additional reserved space
};

// Contains the AVX extension to the XSAVE area, holding upper 
// halves of YMM registers.
struct savefpu_xstate {
    struct xstate_hdr sx_hd;  // Extended state header
    struct ymmacc sx_ymm[16]; // Upper halves of AVX registers (YMM0–YMM15)
};
// Full FPU/SIMD state including AVX. This combines x87, SSE, and AVX registers.
struct savefpu_ymm {
    struct envxmm sv_env; // Environment state (FPU + SSE)
    struct {
        struct fpacc87 fp_acc;
        int8_t fp_pad[6];            // Padding
    } sv_fp[8];                      // x87 accumulators
    struct xmmacc sv_xmm[16];        // SSE registers (XMM)
    uint8_t sv_pad[96];              // Reserved padding
    struct savefpu_xstate sv_xstate; // AVX state (upper halves of YMM)
} __attribute__((aligned(64)));      // Alignment for AVX support

// Holds the hardware debug registers, which are used for setting hardware breakpoints.
struct __dbreg64 {
    // This represents the PS4 debug registers
    // dr[0-3]: debug address registers
    // dr[4-5]: reserved
    // dr[6]: debug status
    // dr[7]: debug control
    // dr[8-15]: reserved
    uint64_t dr[16];
};
// Packet structure used to send breakpoint or interrupt state to a remote debugger client.
struct debug_interrupt_packet {
    uint32_t lwpid;             // Lightweight process (thread) ID
    uint32_t status;            // Wait status (e.g., trap signal)
    char tdname[40];            // Thread name (from LWP info)
    struct __reg64 reg64;       // General-purpose register state (RAX, RIP, etc.)
    struct savefpu_ymm savefpu; // FPU/SIMD/AVX state
    struct __dbreg64 dbreg64;   // Hardware debug registers
} __attribute__((packed));      // No padding between fields (for compact wire format)

extern int g_debugging;
extern struct server_client *curdbgcli;
extern struct debug_context *curdbgctx;

int debug_handle(int fd, struct cmd_packet *packet);
int connect_debugger(struct debug_context *dbgctx, struct sockaddr_in *client);
int debug_attach_handle(int fd, struct cmd_packet *packet);
int debug_detach_handle(int fd, struct cmd_packet *packet);
int debug_breakpt_handle(int fd, struct cmd_packet *packet);
int debug_watchpt_handle(int fd, struct cmd_packet *packet);
int debug_threads_handle(int fd, struct cmd_packet *packet);
int debug_stopthr_handle(int fd, struct cmd_packet *packet);
int debug_resumethr_handle(int fd, struct cmd_packet *packet);
int debug_getregs_handle(int fd, struct cmd_packet *packet);
int debug_setregs_handle(int fd, struct cmd_packet *packet);
int debug_getfpregs_handle(int fd, struct cmd_packet *packet);
int debug_setfpregs_handle(int fd, struct cmd_packet *packet);
int debug_getdbregs_handle(int fd, struct cmd_packet *packet);
int debug_setdbregs_handle(int fd, struct cmd_packet *packet);
int debug_stopgo_handle(int fd, struct cmd_packet *packet);
int debug_thrinfo_handle(int fd, struct cmd_packet *packet);
int debug_singlestep_handle(int fd, struct cmd_packet *packet);
void debug_cleanup(struct debug_context *dbgctx);


#endif
