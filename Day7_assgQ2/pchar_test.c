#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>

void sigint_handler(int sig)
{
    printf("SIGINT caught %d\n",sig);


}

int main()
{
    int fd,ret;
    char buf[32];
    struct sigaction sa;
    memset(&sa,0,sizeof(struct sigaction));
    sa.sa_handler = sigint_handler;
    ret = sigaction(SIGINT,&sa,NULL);
    if(ret < 0)
    {
        perror("sigaction() failed\n");
        _exit(1);
    }
   fd = open("/dev/pchar2",O_RDWR);
   if(fd < 0)
   {
    perror("open () failed.\n");
        _exit(1);

   }
   strcpy(buf,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");

     ret = write(fd,buf,strlen(buf));//buf to write data from
     if(ret < 0)
     {
        perror("write () error\n");
        return ret;

     }

    printf("write() returns no of bytes %d\n",ret);

    strcpy(buf,"1234567890");
    ret = write(fd,buf,strlen(buf));
    if(ret < 0)
    {
        perror("write() error\n");
        return ret;
    }
    printf("write() returns no of bytes %d\n",ret);

    strcpy(buf,"+-/*");

    ret  = write(fd,buf,strlen(buf));
    
    if(ret < 0)
        perror("write() error");

    printf("write() returned  %d\n",ret);
    
    close(fd);








return 0;

}