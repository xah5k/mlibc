#include "/home/ah5k/osdev/base/arch/x86_64/syscallidx.h"
#include <stdint.h>
#include <ah5kos.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mlibc/allocator.hpp>

extern "C" uint64_t dosyscall(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,  uint64_t arg5);

// implements os specifics

extern "C" {
    // start fb
    typedef struct {
        Framebuffer* dest;
        Framebuffer* src;
        uint64_t x;
        uint64_t y;
        uint64_t w;
        uint64_t h;
    } FbDrawPartArgs;
    typedef struct {
        Framebuffer* fb;
        char c;
        int x;
        int y;
        uint32_t fg_color;
        uint32_t bg_color;
    } FbPutcAtArgs;

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
    KSTATUS OsPutcAtFb(Framebuffer* fb, char c, int x, int y, uint32_t fg, uint32_t bg) {
        FbPutcAtArgs args;
        args.fb = fb;
        args.c = c;
        args.x = x;
        args.y = y;
        args.fg_color = fg;
        args.bg_color = bg;
        return (KSTATUS)dosyscall(OS_FBPUTCAT, (uint64_t)&args, 0, 0, 0, 0);
    }
    KSTATUS OsDrawTextAtFb(Framebuffer* fb, const char* text, int x, int y, uint32_t fg, uint32_t bg) {
        const char* ptr = text;
        int X = x;
        int Y = y;
        while (*ptr != '\0') {
            OsPutcAtFb(fb, *ptr, X, Y, fg, bg);
            ptr++;
            X += 8;
        }
        return KSUCCESS;
    }
    KSTATUS OsGetFbInfo(Framebuffer* buf) {
        KSTATUS s = (KSTATUS)dosyscall(OS_FBGETINFO, (uint64_t)buf, 0, 0, 0, 0);
        return KSUCCESS;
    }
    // end fb

    // start time
    KSTATUS OsSleepMs(uint64_t ms) {
        KSTATUS s = KSUCCESS;
        uint64_t dms = dosyscall(OS_SLEEPMS, ms, 0, 0, 0, 0);
        if (dms == ms) s = KSUCCESS;
        return s;
    }
    KSTATUS OsSleep(uint64_t s) {
        return OsSleepMs(1000 * s);
    }
    // end time

    // start signal
    KSTATUS OsSignalRegister(void* f, int SigIdx) {
        uint64_t r = dosyscall(OS_SIGREGISTER, SigIdx, (uint64_t)f, 0, 0, 0);
        return (KSTATUS)r;
    } 
    // end signal

    // start msg passing
    OsMessage* OsMsgGet(uint64_t* LenOut, uint64_t MaxLen) {
        OsMessage* m = (OsMessage*)getAllocator().allocate(sizeof(OsMessage) + MaxLen);
        if (!m) {
            return NULL;
        }
        uint64_t r = dosyscall(OS_GETMSGQUEUE, (uint64_t)m, MaxLen, 0, 0, 0);
        if ((KSTATUS)r != KSUCCESS) {
            getAllocator().free(m);
            return NULL;
        }
        if (LenOut) *LenOut = m->Length;
        return m;
    }
    KSTATUS OsMsgSend(OsMessage* msg) {
        if (!msg) return KINVALID;
        return (KSTATUS)dosyscall(OS_SENDMSG, (uint64_t)msg, 0, 0, 0, 0);
    }
    // end msg

    // shared mem
    KSTATUS OsSharedMapA(uint64_t pid, void* addr, uint64_t length) {
        if (!addr) return KINVALID;
        return (KSTATUS)dosyscall(OS_SHMAPA, pid, (uint64_t)addr, length, 0, 0);
    }
    KSTATUS OsSharedMapB(uint64_t pid, void* addr, uint64_t length) {
        if (!addr) return KINVALID;
        return (KSTATUS)dosyscall(OS_SHMAPB, pid, (uint64_t)addr, length, 0, 0);
    }
    // end shared mem

    // process
    KSTATUS OsYield() {
        return (KSTATUS)dosyscall(OS_YIELD, 0, 0, 0, 0, 0);
    }
    
    // end process
}