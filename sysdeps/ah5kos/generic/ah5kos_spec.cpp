#include "/home/ah5k/osdev/base/arch/x86_64/syscallidx.h"
#include <stdint.h>
#include "../include/ah5kos.h"
#include <stddef.h>

extern "C" uint64_t dosyscall(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,  uint64_t arg5);

extern "C" {
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
    KSTATUS OsGetFbInfo(Framebuffer* buf) {
        KSTATUS s = (KSTATUS)dosyscall(OS_FBGETINFO, (uint64_t)buf, 0, 0, 0, 0);
        return KSUCCESS;
    }
}