#pragma once
#ifndef _DEBUG_H
#define _DEBUG_H

#include <ps4.h>
#include "./protocol.h"
#include "./net.h"
#include "./ptrace.h"
/*========================================================================*
 *                      Debugger Packet Size Macro                       *
 *========================================================================*/

// DEBUG_INTERRUPT_PACKET_SIZE:
// Defines the size (in bytes) of the `debug_interrupt_packet` structure
// This size is critical for ensuring proper alignment and data transmission during remote debugger operations
#define DEBUG_INTERRUPT_PACKET_SIZE 0x4A0

/*========================================================================*
 *                     Debug Register DR7 Control Macros                 *
 *========================================================================*/

// DBREG_DR7_DISABLE:
// Disables the debug breakpoint for a specific debug register (DRx)
#define DBREG_DR7_DISABLE 0x00

// DBREG_DR7_LOCAL_ENABLE:
// Enables local breakpoint detection for a specific debug register (DRx)
// Local breakpoints are triggered only by the current task/thread
#define DBREG_DR7_LOCAL_ENABLE 0x01

// DBREG_DR7_GLOBAL_ENABLE:
// Enables global breakpoint detection for a specific debug register (DRx)
// Global breakpoints can be triggered by any task/thread in the system
#define DBREG_DR7_GLOBAL_ENABLE 0x02

/*========================================================================*
 *                  Debug Register DR7 Length Macros                     *
 *========================================================================*/
#define DBREG_DR7_LEN_1 0x00 // Sets the breakpoint length to 1 byte
#define DBREG_DR7_LEN_2 0x01 // Sets the breakpoint length to 2 bytes
#define DBREG_DR7_LEN_4 0x03 // Sets the breakpoint length to 4 bytes
#define DBREG_DR7_LEN_8 0x02 // Sets the breakpoint length to 8 bytes

/*========================================================================*
 *                Debug Register DR7 Access Type Macros                  *
 *========================================================================*/
#define DBREG_DR7_EXEC   0x00 // Configures the breakpoint to trigger on instruction execution
#define DBREG_DR7_WRONLY 0x01 // Configures the breakpoint to trigger on memory writes only
#define DBREG_DR7_RDWR   0x03 // Configures the breakpoint to trigger on both memory reads and writes

/*========================================================================*
 *               Debug Register DR7 Bitmask and Control Macros           *
 *========================================================================*/

// DBREG_DR7_MASK(i):
// Generates a mask for enabling/disabling breakpoints for a specific debug register `DRi`
// 
// Parameters:
// - `i`: Index of the debug register (0-3)
// 
// Returns:
// - A bitmask for controlling breakpoints in DRi, including access type and enable flags
#define DBREG_DR7_MASK(i) ((uint64_t)(0xf) << ((i) * 4 + 16) | 0x3 << (i) * 2)

// DBREG_DR7_SET(i, len, access, enable):
// Configures a specific debug register `DRi` with length, access type, and enable flags
//
// Parameters:
// - `i`: Index of the debug register (0-3)
// - `len`: Length of the memory region to monitor (use `DBREG_DR7_LEN_*` macros)
// - `access`: Access type (use `DBREG_DR7_EXEC`, `DBREG_DR7_WRONLY`, or `DBREG_DR7_RDWR` macros)
// - `enable`: Enable flag (`DBREG_DR7_LOCAL_ENABLE` or `DBREG_DR7_GLOBAL_ENABLE`)
//
// Returns:
// A properly formatted value for DR7 configuration
#define DBREG_DR7_SET(i, len, access, enable) ((uint64_t)((len) << 2 | (access)) << ((i) * 4 + 16) | (enable) << (i) * 2)

// DBREG_DR7_GD:
// Global Debug flag in DR7 When set, it indicates that a debug exception 
// occurred due to a global condition
#define DBREG_DR7_GD 0x2000


// DBREG_DR7_ENABLED(d, i):
// Checks if a specific debug register `DRi` is enabled in the DR7 control register
// 
// Parameters:
// - `d`: The value of the DR7 control register
// - `i`: Index of the debug register (0-3)
// 
// Returns:
// - Non-zero if the debug register `DRi` is enabled, 0 otherwise
#define DBREG_DR7_ENABLED(d, i) (((d) & 0x3 << (i) * 2) != 0)

// DBREG_DR7_ACCESS(d, i):
// Extracts the access type (execute, write-only, or read/write) for a specific debug register `DRi`
// 
// Parameters:
// - `d`: The value of the DR7 control register
// - `i`: Index of the debug register (0-3)
// 
// Returns:
// - The access type for `DRi` (use `DBREG_DR7_EXEC`, `DBREG_DR7_WRONLY`, or `DBREG_DR7_RDWR` macros)
#define DBREG_DR7_ACCESS(d, i) ((d) >> ((i) * 4 + 16) & 0x3)

// DBREG_DR7_LEN(d, i):
// Extracts the length configuration for a specific debug register `DRi`
// 
// Parameters:
// - `d`: The value of the DR7 control register
// - `i`: Index of the debug register (0-3)
// 
// Returns:
// - The length configuration for `DRi` (use `DBREG_DR7_LEN_*` macros)
#define DBREG_DR7_LEN(d, i) ((d) >> ((i) * 4 + 18) & 0x3)

// DBREG_DRX(d, x):
// Accesses a specific debug register (DR0-DR7) by index
// 
// Parameters:
// - `d`: Pointer to a struct containing the debug registers (`__dbreg64`)
// - `x`: Index of the debug register to access (0-7)
// 
// Returns:
// - The value of the specified debug register
#define DBREG_DRX(d, x) ((d)->dr[(x)])

#define DEBUG_PORT 755

/*========================================================================*
 *                    CPU Register Context Structures                    *
 *========================================================================*/

// Struct represents x86-64 General Purpose Registers (64-bit)
// Mirrors hardware register layout during interrupts/exceptions
struct __reg64 {
    uint64_t r_r15;     // General purpose register 15
    uint64_t r_r14;     // General purpose register 14
    uint64_t r_r13;     // General purpose register 13
    uint64_t r_r12;     // General purpose register 12
    uint64_t r_r11;     // General purpose register 11
    uint64_t r_r10;     // General purpose register 10
    uint64_t r_r9;      // General purpose register 9
    uint64_t r_r8;      // General purpose register 8
    uint64_t r_rdi;     // Destination index (DI) register
    uint64_t r_rsi;     // Source index (SI) register
    uint64_t r_rbp;     // Base pointer (BP) register
    uint64_t r_rbx;     // Base register (BX)
    uint64_t r_rdx;     // Data register (DX)
    uint64_t r_rcx;     // Counter register (CX)
    uint64_t r_rax;     // Accumulator register (AX)

    // Exception handling metadata
    uint32_t r_trapno;  // Trap number that caused exception
    uint16_t r_fs;      // Segment register: File selector (FS)
    uint16_t r_gs;      // Segment register: Graphics selector (GS)
    uint32_t r_err;     // Error code associated with exception

    // Legacy segment registers
    uint16_t r_es;      // Extra segment register (ES)
    uint16_t r_ds;      // Data segment register (DS)

    // Instruction pointer and control registers
    uint64_t r_rip;     // Instruction pointer (RIP)
    uint64_t r_cs;      // Code segment register (CS)
    uint64_t r_rflags;  // Processor flags register (RFLAGS)
    uint64_t r_rsp;     // Stack pointer (RSP)
    uint64_t r_ss;      // Stack segment register (SS)
};

/*========================================================================*
 *                Floating Point Unit (FPU) State Structures             *
 *========================================================================*/

// x87 FPU 80-bit extended precision register (ST0-ST7)
struct fpacc87 {
    // 80-bit extended precision value (IEEE 754)
    uint8_t fp_bytes[10];
};

// SSE 128-bit XMM register (XMM0-XMM15)
struct xmmacc {
    // 128-bit packed data (single/double floats, integers)
    uint8_t xmm_bytes[16];
};

// AVX upper 128-bits of YMM registers (YMM0-YMM15)
struct ymmacc {
    // Upper 128 bits of 256-bit YMM register
    uint8_t ymm_bytes[16];
};

// FPU Control/Status Environment (MXCSR etc.)
// This matches PS4-specific FXSAVE/FXRSTOR layout
struct envxmm {
    uint16_t en_cw;         // FPU Control Word (precision/rounding control)
    uint16_t en_sw;         // FPU Status Word (condition codes/exceptions)
    uint8_t en_tw;          // FPU Tag Word (register validity bits)
    uint8_t en_zero;        // Reserved padding (set to 0)
    uint16_t en_opcode;     // Last FPU opcode executed (11 bits) + 5 reserved
    uint64_t en_rip;        // FPU Instruction Pointer (RIP at last FP op)
    uint64_t en_rdp;        // FPU Data Pointer (operand address)
    uint32_t en_mxcsr;      // SSE Control/Status Register (MXCSR)
    uint32_t en_mxcsr_mask; // MXCSR_MASK (valid bits for MXCSR writes)
};

// Legacy FPU+SSE Save Area (FXSAVE format)
// 16-byte aligned for XMM access
struct savefpu {
    struct envxmm sv_env;        // Control/status environment
    struct {
        struct fpacc87 fp_acc;   // x87 ST register value
        uint8_t fp_pad[6];       // Padding to 16-byte boundary
    } sv_fp[8];                  // x87 registers ST0-ST7
    struct xmmacc sv_xmm[16];    // SSE XMM0-XMM15 registers
    uint8_t sv_pad[96];          // Reserved (AVX state would follow)
} __attribute__((aligned(16)));  // Required for XMM memory operations


/*========================================================================*
 *                 Extended Processor State (AVX/XSAVE)                  *
 *========================================================================*/

 // XSAVE Header (PS4 uses XSAVEOPT for context saving) 
struct xstate_hdr {
    uint64_t xstate_bv;        // Bitmask of enabled state components
    uint8_t xstate_rsrv0[16];  // Reserved
    uint8_t xstate_rsrv[40];   // Future expansion
};

// Extended AVX state component 
struct savefpu_xstate {
    struct xstate_hdr sx_hd;    // XSAVE header
    struct ymmacc sx_ymm[16];   // Upper 128 bits of YMM0-YMM15
};

// Full FPU+SSE+AVX State (XSAVE area)
// PS4-specific 64-byte alignment required by XSAVE
struct savefpu_ymm {
    struct envxmm sv_env;       // Base FPU/SSE environment
    struct {
        struct fpacc87 fp_acc; // x87 ST register value
        int8_t fp_pad[6];      // Padding to 16-byte boundary
    } sv_fp[8];                // x87 registers ST0-ST7
    struct xmmacc sv_xmm[16];  // SSE XMM0-XMM15 registers
    uint8_t sv_pad[96];        // Reserved space for legacy alignment
    struct savefpu_xstate sv_xstate; // Extended state (AVX YMM)
} __attribute__((aligned(64))); // Required by XSAVE instruction


/*========================================================================*
 *                     Debug Register Context Structure                  *
 *========================================================================*/

// x86 Debug Registers (DR0-DR7)
// PS4-specific DR4/DR5 reserved in long mode
struct __dbreg64 {
    // Debug registers (usage depends on mode)
    // dr[0-3]: DR0-DR3 - Breakpoint linear addresses
    // dr[4-5]: DR4-DR5 - Reserved in x86-64
    // dr[6]:   DR6     - Debug status register
    // dr[7]:   DR7     - Debug control register
    // dr[8-15]: Unused
    uint64_t dr[16];
};

/*========================================================================*
 *                  Debug Interrupt Context Packet                       *
 *========================================================================*/

// Complete Process State Snapshot (packed for network transmission)
// Packed to 1-byte alignment for network protocol
struct debug_interrupt_packet {
    uint32_t lwpid;             // Lightweight Process ID (thread ID)
    uint32_t status;            // Process status flags
    char tdname[40];            // Thread name 
    struct __reg64 reg64;       // General-purpose registers
    struct savefpu_ymm savefpu; // Complete FPU/SSE/AVX state
    struct __dbreg64 dbreg64;   // Debug registers state
} __attribute__((packed));      // Critical for network transmission alignment


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