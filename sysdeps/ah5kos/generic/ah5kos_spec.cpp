#include "/home/ah5k/osdev/base/arch/x86_64/syscallidx.h"
#include <stdint.h>
#include "../include/ah5kos.h"
#include <stddef.h>

extern "C" uint64_t dosyscall(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,  uint64_t arg5);

extern "C" {
    typedef struct {
        Framebuffer* dest;
        Framebuffer* src;
        uint64_t x;
        uint64_t y;
        uint64_t w;
        uint64_t h;
    } FbDrawPartArgs;
    Framebuffer* OsCreateFb(uint32_t width, uint32_t height) {
        uint64_t f = dosyscall(OS_FBCREATE, (uint64_t)width, (uint64_t)height, 0, 0, 0);
        return (Framebuffer*)f;
    }
    KSTATUS OsFreeFb(Framebuffer* fb) {
        return (KSTATUS)dosyscall(OS_FBFREE, (uint64_t)fb, 0, 0, 0, 0);
    }
    KSTATUS OsDrawFb(Framebuffer* destfb, Framebuffer* srcfb, int x, int y) {
        return (KSTATUS)dosyscall(OS_FBDRAW, (uint64_t)destfb, (uint64_t)srcfb, (uint64_t)x, (uint64_t)y, 0);
    }
    KSTATUS OsDrawFbPart(Framebuffer* destfb, Framebuffer* srcfb, int x, int y, int w, int h) {
        FbDrawPartArgs args;
        args.dest = destfb;
        args.src = srcfb;
        args.x = (uint64_t)x;
        args.y = (uint64_t)y;
        args.w = (uint64_t)w;
        args.h = (uint64_t)h;
        return (KSTATUS)dosyscall(OS_FBDRAWPART, (uint64_t)&args, 0, 0, 0, 0);
    }
    KSTATUS OsGetFbInfo(Framebuffer* buf) {
        KSTATUS s = (KSTATUS)dosyscall(OS_FBGETINFO, (uint64_t)buf, 0, 0, 0, 0);
        return KSUCCESS;
    }
    KSTATUS OsSleepMs(uint64_t ms) {
        KSTATUS s = KSUCCESS;
        uint64_t dms = dosyscall(OS_SLEEPMS, ms, 0, 0, 0, 0);
        if (dms == ms) s = KSUCCESS;
        return s;
    }
    KSTATUS OsSleep(uint64_t s) {
        return OsSleepMs(1000 * s);
    }
}