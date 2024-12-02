#ifndef __PCHAR_IOCTL_H
#define __PCHAR_IOCTL_H

#include <linux/ioctl.h>

typedef struct devinfo {
    short size;
    short len;
    short avail;
}devinfo_t;

#define FIFO_CLEAR      _IO('x', 1)
#define FIFO_GETINFO    _IOR('x', 2, devinfo_t)
#define FIFO_RESIZE     _IOW('x', 3, unsigned int)

#endif
