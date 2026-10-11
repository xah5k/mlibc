#pragma once

#ifdef __cplusplus 
extern "C" {
#endif
#include <stdint.h>

// used in kernel internally to represent a point
typedef struct {
    uint64_t x;
    uint64_t y;
} Point;

// mouse
typedef struct {
    Point RawPos;
    int LeftClickPress;
    int RightClickPress;
    int MiddleClickPress;
} KeDevMousePacket;

// reading the rtc
typedef struct {
    uint8_t Seconds;
    uint8_t Minutes;
    uint8_t Hours;
    uint8_t Days;
    uint8_t Month;
    uint8_t Year; // short hand as in 0-99
} KeDevClockWallTime;

typedef struct {
    uint64_t ptr; // userspace wont be able to write to it anyway
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint32_t scanline;
    uint64_t bpp;
} Framebuffer;

// wrapper
typedef struct {
    Framebuffer* fb;
} KeDevFbInfo;

// kstatus
typedef enum {
    KSUCCESS,
    KFAIL,
    KUNSUPPORTED,
    KINVALID,
    KOOMERR,
    KHUNG,
    KRESEND,
    KDOUBLEFREE,
    KRANGE
} KSTATUS;
// end kstatus

// raw syscall
uint64_t dosyscall(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,  uint64_t arg5);
// end raw syscall

// fb
Framebuffer* OsCreateFb(uint32_t width, uint32_t height);
KSTATUS OsFreeFb(Framebuffer* fb);
KSTATUS OsDrawFb(Framebuffer* destfb, Framebuffer* srcfb, int x, int y);
KSTATUS OsDrawFbPart(Framebuffer* destfb, Framebuffer* srcfb, int x, int y, int w, int h);
KSTATUS OsGetFbInfo(Framebuffer* buf);
KSTATUS OsPutcAtFb(Framebuffer* fb, char c, int x, int y, uint32_t fg, uint32_t bg);
KSTATUS OsDrawTextAtFb(Framebuffer* fb, const char* text, int x, int y, uint32_t fg, uint32_t bg); // doesnt parse newlines or any special control characters
// end fb

// kb
typedef enum {
    KEY_ESCAPE = 0xE0,
    KEY_F1 = 0xF0,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_NUMLCK,
    KEY_SCRLCK,
    KEY_NM7 = '7',
    KEY_NM8,
    KEY_NM9,
    KEY_NMMINUS = '-',
    KEY_NM4 = '4',
    KEY_NM5,
    KEY_NM6,
    KEY_NMPLUS = '+',
    KEY_NM1 = '1',
    KEY_NM2 = '2',
    KEY_NM3 = '3',
    KEY_NM0 = '0',
    KEY_NMDOT = '.',
    KEY_F11 = 252,
    KEY_F12
} KbdTranslExtraKey;
// end kb

// time
KSTATUS OsSleepMs(uint64_t ms);
KSTATUS OsSleep(uint64_t s);
// end time

// signal
typedef KSTATUS (*OsSignalHdlFunc)(int SigIdx);
KSTATUS OsSignalRegister(void* f, int SigIdx);
// KSTATUS OsSignalBlock(int SigIdx);
// KSTATUS OsSignalUnblock(int SigIdx);
// KSTATUS OsSignalSend(int Pid, int SigIdx);

// end signal

// msg passing
// sync with sched/ipc/signal.h
#define SIGNAL_RECEIVEMSG 45
typedef struct {
    uint64_t FromPid;
    uint64_t ToPid;
    uint64_t Length;
    void* Reserved; // cant be read as it points to smth not mapped in the user program.
} OsMessage;
OsMessage* OsMsgGet(uint64_t* LenOut, uint64_t MaxLen); // gets the most latest msg
KSTATUS OsMsgSend(OsMessage* msg);
// end msg

// shared mem
// difference between A and B
// A maps 'addr' from 'pid' address space into the syscall callers address space
// B maps 'addr' from syscall callers address space into 'pid' address space
KSTATUS OsSharedMapA(uint64_t pid, void* addr, uint64_t length);
KSTATUS OsSharedMapB(uint64_t pid, void* addr, uint64_t length);
// end shared mem

// process
#define OS_PLOW 0
#define OS_PMEDIUM 16
#define OS_PHIGH 32
KSTATUS OsYield();
uint64_t OsGetSchedPr();
KSTATUS OsSetSchedPr(uint64_t pr);
// end process
#ifdef __cplusplus 
}
#endif