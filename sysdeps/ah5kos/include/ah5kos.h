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
KSTATUS OsGetFbInfo(Framebuffer* buf);
// end fb

#ifdef __cplusplus 
}
#endif