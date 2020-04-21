#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


#define DEVICE "/dev/kjDevice"

int main()
{
    int i,fd;
    char opt,write_buff[10],read_buff[10];

    fd = open(DEVICE,O_RDWR);

    if(fd == -1)
    {
        printf("file %s either does not exist or been locked by other process\n",DEVICE);

        exit(-1);
    }
    printf("Select : r -> read from device,w -> write to device\n");
    scanf("%c",&opt);
    switch(opt)
    {
        case 'w':
            printf("Enter the data: ");
            scanf(" %[^\n]",write_buff);
            write(fd,write_buff,sizeof(write_buff));
            break;
        case 'r':
            read(fd,read_buff,sizeof(read_buff));
            printf("device: %s\n", read_buff);
            break;

        default:
            printf("inavlid option\n");
            break;
    }
    close(fd);
    return 0;
}