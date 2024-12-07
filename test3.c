#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[512];

int itoa (int num)
{
    // printf(1, "int num = %d\n", num);
    if(num == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return 1;
    }

    int i = 0;
    char bufrev[512];
    while(num > 0)
    {
        char t = (char)(num % 10 + 48);
        bufrev[i] = t;
        num /= 10;
        i++;
    }
    buf[i] = '\0';
    int len = i;
    for (int j = 0; j < len; j++)
    {
        i--;
        buf[j] = bufrev[i];
    }
    return len;
}

int 
main(int argc, char **argv) 
{
    // init buf
    for (int i = 0; i < 512; i++)
    {
        buf[i] = '\0';
    }

    // init file
    unlink("data.txt");
    int fd0 = open("data.txt", O_CREATE | O_RDWR);
    if(fd0 < 0)
        printf(1, "open failed");

    int num = 0;
    printf(1, "num = %d\n", num);
    // save num as string to buf
    itoa(num);
    // write to file fd
    if (write(fd0, buf, strlen(buf)) < 0) 
    {
        printf(1, "write error\n");
        exit();
    }
    close(fd0);

    int fd = open("data.txt", O_RDWR);
    if(fd < 0)
        printf(1, "open failed");
    
    int pid;
    int loops = atoi(argv[1]);

    for (int i = 0; i < loops; i++)
    {
        int fd = open("data.txt", O_RDWR);
        if(fd < 0)
            printf(1, "open failed");
            
        if (read(fd, buf, sizeof(buf)) < 0)
        {
            printf(1, "read error\n");
            exit();
        } 
        // printf(1, "buf = %s\n", buf);
        num = atoi(buf); 
        num++;
        printf(1, "num = %d\n", num);

        pid = fork();
        if(pid < 0)
        {
            printf(1, "fork failed\n");
            exit();
        }
        if(pid == 0)
        {
            // itoa(num);
            // if (write(fd, buf, strlen(buf)) < 0) 
            // {
            //     printf(1, "write error\n");
            //     exit();
            // }
            close(fd);
            exit();
        }
        else 
        {
            // num = i; 
            // itoa(num);
            // if (write(fd, buf, strlen(buf)) < 0) 
            // {
            //     printf(1, "write error\n");
            //     exit();
            // }
            
            wait();
            // close(fd);
        }
    }

    // close(fd);
    exit();
}