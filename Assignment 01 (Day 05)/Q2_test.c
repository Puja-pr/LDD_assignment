#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int fd, ret, newfd;
    char buf[32];

    fd = open("/dev/pchar", O_RDWR);
    if(fd < 0){
        perror("open() failed");
        _exit(1);
    }

    strcpy(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ret = write(fd, buf, strlen(buf));
    printf("write() returned: %d\n", ret);

    strcpy(buf, "1234567890");
    ret = write(fd, buf, strlen(buf));
    printf("write() returned: %d\n", ret);

    strcpy(buf, "+-*/");
    ret = write(fd, buf, strlen(buf));
    if(ret < 0)
        perror("write() error");
    printf("write() returned: %d\n", ret);

    ret = lseek(fd, 0, SEEK_SET);
    printf("lseek() returned pos: %d\n", ret);

    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, 26);
    printf("read() returned : %d --> %s\n", ret, buf);

    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, 10);
    printf("read() returned : %d --> %s\n", ret, buf);

    memset(buf, 0, sizeof(buf));
    ret = read(fd, buf, 4);
    printf("read() returned : %d --> %s\n", ret, buf);

    close(fd);
    return 0;
}