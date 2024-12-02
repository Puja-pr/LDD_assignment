#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE_NAME "/dev/pchar0"  
#define PCHAR_CLEAR_BUFFER _IO('p', 1)
#define PCHAR_GET_INFO _IOR('p', 2, struct buffer_info)

struct buffer_info {
    unsigned int used_bytes;
    unsigned int available_bytes;
};

int main() {
    int fd;
    struct buffer_info info;

    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return EXIT_FAILURE;
    }

    /*
    if (ioctl(fd, PCHAR_CLEAR_BUFFER) < 0) {
        perror("Failed to clear the buffer");
        close(fd);
        return EXIT_FAILURE;
    }
    printf("Buffer cleared successfully.\n");
	*/
	
    if (ioctl(fd, PCHAR_GET_INFO, &info) < 0) {
        perror("Failed to get buffer info");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Buffer Info:\n");
    printf("Used Bytes: %u\n", info.used_bytes);
    printf("Available Bytes: %u\n", info.available_bytes);

    close(fd);
    return 0;
}
