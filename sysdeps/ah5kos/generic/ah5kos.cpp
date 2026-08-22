#include <mlibc/internal-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <errno.h>
#include <stddef.h>
#include <abi-bits/pid_t.h>
#include <sys/utsname.h>
#include "/home/ah5k/osdev/base/arch/x86_64/syscallidx.h"
#include <termios.h>
#include <sys/wait.h>
#include <asm-generic/fcntl.h>
#include <poll.h>

// sync with fs/vfs.h
#define VFS_HANDLE_STDOUT 1
#define VFS_HANDLE_STDERR 2
#define VFS_HANDLE_STDIN 0

extern "C" uint64_t dosyscall(uint64_t sys_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4,  uint64_t arg5) {
    uint64_t ret;
    register uint64_t r8_val __asm__("r8") = arg5;

    __asm__ __volatile__ (
        "int $0xFF"
        : "=a" (ret)
        : "a" (sys_num),
          "D" (arg1),
          "S" (arg2),
          "d" (arg3),
          "c" (arg4),
          "r" (r8_val)
        : "memory", "cc"
    );

    return ret;
}

namespace mlibc {
int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window);
int sys_vm_unmap(void *pointer, size_t size);
void sys_exit(int status);
void sys_libc_log(const char *message) {
    dosyscall(OS_CONWRITE, (uint64_t)message, 0, 0, 0, 0);
}

void sys_libc_panic() {
    sys_libc_log("\nmlibc panic\n");
    sys_exit(1);
}

void sys_exit(int status) {
    dosyscall(OS_EXIT, (uint64_t)status, 0, 0, 0, 0);
    while (1) { __builtin_trap(); }
}

int sys_tcb_set(void *pointer) {
    dosyscall(OS_SETFSBASE, (uint64_t)pointer, 0, 0, 0, 0);
    return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
    return sys_vm_map(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0, pointer);
}

int sys_anon_free(void *pointer, size_t size) {
    return sys_vm_unmap(pointer, size);
}

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
    if (count == 0) {
        if (bytes_written) *bytes_written = 0;
        return 0;
    }
    uint64_t r = dosyscall(OS_WRITE, (uint64_t)fd, (uint64_t)buf, (uint64_t)count, 0, 0);
    if (r == (uint64_t)-1) return EBADF;
    *bytes_written = r;
    return 0;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
    uint64_t r = dosyscall(OS_OPEN, (uint64_t)path, 0, 0, 0, 0);
    if (r == (uint64_t)-1) return EBADF;
    *fd = (int)r;
    return 0;
}

// stupid shi for max arg 5 limit
typedef struct {
    void* addr;
    uint64_t length;
    int protect;
    int flags;
    int fd;
    uint64_t offset;
} MmapArgs;

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
    MmapArgs a;
    a.addr = hint;
    a.length = size;
    a.protect = prot;
    a.flags = flags;
    a.fd = fd;
    a.offset = offset;
    uint64_t r = dosyscall(OS_MMAP, (uint64_t)&a, 0, 0, 0, 0);
    *window = (void*)r;
    return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
    uint64_t r = dosyscall(OS_MUNMAP, (uint64_t)pointer, (uint64_t)size, 0, 0, 0);
    return (int)r;
}

int sys_close(int fd) {
    uint64_t r = dosyscall(OS_CLOSE, (uint64_t)fd, 0, 0, 0, 0);
    if (r == (uint64_t)-1) return ENOENT;
    return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
    if (count == 0) {
        if (bytes_read) *bytes_read = 0;
        return 0;
    }
    uint64_t r = dosyscall(OS_READ, (uint64_t)fd, (uint64_t)buf, (uint64_t)count, 0, 0);
    if (r == (uint64_t)-1) return EBADF;
    *bytes_read = r;
    return 0;
}


int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
    uint64_t r = dosyscall(OS_SEEK, (uint64_t)fd, (uint64_t)offset, (uint64_t)whence, 0, 0);
    if (r == (uint64_t)-1) {
        return ESPIPE;
    } 
    off_t newoff = (off_t)r;
    *new_offset = newoff;
    return 0;
}

int sys_clock_get(int clock_id, time_t *secs, long *nanosecs) {
    uint64_t s;
    uint64_t ns;
    uint64_t r = dosyscall(OS_GETCLOCK, (uint64_t)clock_id, (uint64_t)&s, (uint64_t)&ns, 0, 0);
    if (r == (uint64_t)-1) return ENOSYS;
    *secs = (time_t)s;
    *nanosecs = (long)ns;
    return 0;
}

// stub
int sys_futex_wait(int *pointer, int expected, const struct timespec *timeout) { return ENOSYS; }
int sys_futex_wake(int *pointer) { return ENOSYS; }

// fake a stat struct for good luck
int sys_fstat(int fd, struct stat *st) {
    memset(st, 0, sizeof(struct stat));
    int r = (int)dosyscall(OS_STAT, (uint64_t)fd, (uint64_t)st, 0, 0, 0);
    if (r == (uint64_t)-1) return ENOENT;
    return 0;
}
int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
    switch (fsfdt) {
        case fsfd_target::path: {
            int r = (int)dosyscall(OS_STAT, (uint64_t)path, (uint64_t)statbuf, 0, 0, 0);
            if (r == -1) return ENOENT;
            return 0;
        }
        case fsfd_target::fd: {
            int r = sys_fstat(fd, statbuf);
            return r;
        }
        default: {
            sys_libc_log("warn: sys_stat: stat called with fsfdt not being path or fd.\r\n");
            return ENOSYS;
        }
    }
}

int sys_isatty(int fd) {
    if (fd == VFS_HANDLE_STDERR || fd == VFS_HANDLE_STDOUT) {
        return 0;
    }
    return ENOTTY;
}

void sys_yield() {
    dosyscall(OS_YIELD, 0, 0, 0, 0, 0);
}

pid_t sys_getpid() {
    return (pid_t)dosyscall(OS_GETPID, 0, 0, 0, 0, 0);
}

pid_t sys_getppid() {
    return (pid_t)dosyscall(OS_GETPPID, 0, 0, 0, 0, 0);
}
int sys_kill(int pid, int signal) {
    (void)signal; // no signals in kernel yet it js uses a flag
    return (int)dosyscall(OS_KILL, pid, 0, 0, 0, 0);
}

int sys_getcwd(char *buffer, size_t size) {
    if (!buffer) return EINVAL;
    if (size == 0) return EINVAL;
    uint64_t r = dosyscall(OS_GETCWD, (uint64_t)buffer, (uint64_t)size, 0, 0, 0);
    if (r < 0) return EFAULT;
    return 0;
}

int sys_chdir(const char *path) {
    if (!path) return EINVAL;
    uint64_t r = dosyscall(OS_CHDIR, (uint64_t)path, 0, 0, 0, 0);
    return (int)r;
}

int sys_open_dir(const char* path, int* fd) {
    return sys_open(path, 0, 0, fd);
}

int sys_read_entries(int fd, void* buffer, size_t max_size, size_t* bytes_read) {
    if (!buffer || !bytes_read) return EINVAL;
    uint64_t r = dosyscall(OS_GETDIRENT, (uint64_t)fd, (uint64_t)buffer, (uint64_t)max_size, (uint64_t)bytes_read, 0);
    if (r < 0) return ENOENT;
    *bytes_read = r;
    return 0;
}

int sys_uname(struct utsname *buf) {
    if (!buf) return EINVAL;
    uint64_t r = dosyscall(OS_UNAME, (uint64_t)buf, 0, 0, 0, 0);
    if (r < 0) return ENOSYS;
    return 0;
}
#define	R_OK 4
#define	W_OK 2
#define	X_OK 1
#define	F_OK 0

int sys_access(const char *path, int mode) {
    uint64_t r = dosyscall(OS_ACCESS, (uint64_t)path, mode == F_OK, (mode & R_OK), (mode & W_OK), (mode & X_OK));
    if (r == (uint64_t)-1) {
        return ENOENT;
    }
    if (r == (uint64_t)-2) {
        return EROFS;
    }
    return 0;
}

int sys_umask(mode_t mode, mode_t *old) {
    uint64_t r = dosyscall(OS_UMASK, (uint64_t)mode, (uint64_t)old, 0, 0, 0);
    if (r == (uint64_t)-1) return ENOSYS;
    *old = (mode_t)r;
    return 0;
}

gid_t sys_getgid() {
    return 0;
}
gid_t sys_getegid() {
    return 0;
}
// holy opsec 
uid_t sys_getuid() {
    return 0;
}

uid_t sys_geteuid() {
    return 0;
}

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
    uint64_t r = dosyscall(OS_IOCTL, (uint64_t)fd, (uint64_t)request, (uint64_t)arg, 0, 0);
    if (r < 0) return ENOSYS;
    if (result) *result = r;
    return 0;
}

int sys_pipe(int *fds, int flags) {
    uint64_t r = dosyscall(OS_CRPIPE, (uint64_t)fds, 0, 0, 0, 0);
    if (r == (uint64_t)-1) return EMFILE;
    return 0;
}

int sys_fork(pid_t *child) {
    uint64_t r = dosyscall(OS_FORK, 0, 0, 0, 0, 0);
    if (r == (uint64_t)-1) return ENOMEM;
    if (child) *child = (pid_t)r;
    return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
    uint64_t r = dosyscall(OS_EXECVE, (uint64_t)path, (uint64_t)argv, (uint64_t)envp, 0, 0);
    if (r == (uint64_t)-1) return ENOSYS;
    else return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
    uint64_t r = dosyscall(OS_DUP, (uint64_t)fd, 0, 0, 0, 0);
    if (r == (uint64_t)-1) return EBADF;
    if (newfd) *newfd = (int)r;
    return 0;
}
int sys_dup2(int fd, int flags, int newfd) {
    uint64_t r = dosyscall(OS_DUP2, (uint64_t)fd, (uint64_t)newfd, 0, 0, 0);
    if (r == (uint64_t)-1) return EBADF;
    return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
    uint64_t r = dosyscall(OS_WAIT, (uint64_t)pid, 0, 0, 0, 0);
    if (r == (uint64_t)-1) {
        return ECHILD; 
    }
    if (status) {
        *status = r; 
    }
    if (ret_pid) {
        *ret_pid = pid; 
    }
    return 0;
}

int sys_waitid(idtype_t idtype, id_t id, siginfo_t *info, int options) {
    switch (idtype) {
        case idtype_t::P_PID: {
            int status;
            int ret_pid;
            int r = sys_waitpid(id, &status, 0, nullptr, &ret_pid);
            if (r != 0) return ECHILD;
            if (info) {
                memset(info, 0, sizeof(siginfo_t));
                info->si_signo = SIGCHLD;
                info->si_code = 1;
                info->si_pid = (pid_t)ret_pid;
                info->si_status = (int)r & 0xFF;
            }
            break;
        }
        default: {
            return ENOSYS;
        }
    }
}

int sys_sigaction(int sigidx, const struct sigaction *__restrict sigact, struct sigaction *__restrict sigactold) {
    // todo: we have no tmp signal support as in being able to store old signal handlers
    if (!sigact) return 0;
    
    uint64_t r = dosyscall(OS_SIGREGISTER, sigidx, (uint64_t)sigact->sa_handler, 0, 0, 0);
    return 0;
}

// stub
int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
    for (nfds_t i = 0; i < count; i++) fds[i].revents = 0;
    *num_events = 0;
    return 0;
}


int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
    return 0;
}

int sys_tcgetattr(int fd, struct termios *attr) {
    switch (fd) {
        case VFS_HANDLE_STDIN:
        case VFS_HANDLE_STDERR:
        case VFS_HANDLE_STDOUT: {
            memset((void*)attr, 0, sizeof(termios));
            attr->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                | INLCR | IGNCR | ICRNL | IXON);
            attr->c_oflag &= ~OPOST;
            uint64_t stdoutbuffer;
            uint64_t stdinbuffer;
            uint64_t r = dosyscall(OS_GTERMINFO, (uint64_t)&stdinbuffer, (uint64_t)&stdoutbuffer, 0, 0, 0);
            if (stdinbuffer) attr->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
            else attr->c_lflag &= ~(ECHO | ECHONL | ISIG | IEXTEN);
            attr->c_cflag &= ~(CSIZE | PARENB);
            attr->c_cflag |= CS8;
            attr->c_cc[VINTR] = 0x03;
            attr->c_cc[VEOF] = 0x04;
            attr->c_cc[VMIN] = 1;
            attr->c_cc[VTIME] = 0;
            return 0;
        }
        default: {
            return 0;
        }
    }
    return 0;
}

int sys_tcsetattr(int fd, int opts, const struct termios *attr) {
    switch (fd) {
        case VFS_HANDLE_STDIN:
        case VFS_HANDLE_STDERR:
        case VFS_HANDLE_STDOUT: {
            if (!(attr->c_lflag & ICANON)) {
                uint64_t r = dosyscall(OS_STERMINFO, 0, 1, 0, 0, 0);
            } else {
                uint64_t r = dosyscall(OS_STERMINFO, 0, 0, 0, 0, 0);
            }
            break;
        }
    }
    return 0;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
    switch (request) {
        case F_GETFD: {
            *result = 0;
            return 0; // no support for it yet
        }
        case F_SETFD: {
            *result = 0;
            return 0; // todo
        }
        case F_GETFL: {
            *result = O_RDWR;
            return 0;
        }
        case F_SETFL: {
            *result = 0;
            return 0; // todo
        }
        default: {
            *result = 0;
            return 0;
        }
    }
    return 0;
}

}// namespace mlibc