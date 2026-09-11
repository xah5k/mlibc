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

typedef enum {
    KSUCCESS,
    KFAIL,
    KUNSUPPORTED,
    KINVALID,
    KOOMERR,
    KHUNG,
    KRESEND,
    KDOUBLEFREE
} KSTATUS;

// fb
Framebuffer* OsCreateFb(uint32_t width, uint32_t height);
KSTATUS OsFreeFb(Framebuffer* fb);
KSTATUS OsDrawFb(Framebuffer* destfb, Framebuffer* srcfb, int x, int y);
KSTATUS OsDrawFbPart(Framebuffer* destfb, Framebuffer* srcfb, int x, int y, int w, int h);
KSTATUS OsGetFbInfo(Framebuffer* buf);
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
KSTATUS OsSignalRegister(OsSignalHdlFunc f, int SigIdx);
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
// end msg

#ifdef __cplusplus 
}
#endif