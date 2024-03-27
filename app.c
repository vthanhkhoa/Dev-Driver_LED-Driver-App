#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define FILENAME            "/dev/led_ioctl"

#define MAGIC_NO            100
#define GET_LED_STATE       _IOW(MAGIC_NO, 0, char)
#define LED_ON              _IOW(MAGIC_NO, 1, char)
#define LED_OFF             _IOW(MAGIC_NO, 2, char)
#define LED_BLINK           _IOW(MAGIC_NO, 3, char)

char val[5];
int fd;
int main()
{
    while (1)
    {
        printf("What do you want? \n\t0. Get led's state \t1. On led\n\t2. Off led \t\t3. Blink led\n");
        fgets(val, sizeof(val), stdin);

        fd = open(FILENAME, O_RDWR);
        switch (val[0])
        {
        case '0':
            ioctl(fd, GET_LED_STATE, val[0]);
            break;
        case '1':
            printf("Led will be ON\n");
            ioctl(fd, LED_ON, val[0]);
            break;
        case '2':
            printf("Led will be OFF\n");
            ioctl(fd, LED_OFF, val[0]);
            break;
        case '3':
            printf("3\n");
            ioctl(fd, LED_BLINK, val[0]);
            break;
        default:
            printf("Not support!\n");
        }
        close(fd);
    }

    return 0;
}