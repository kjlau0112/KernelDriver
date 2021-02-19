#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


#define DEVICE "/dev/kjDevice"
#define IOCTL_SETMIXED _IO(239,0)
#define IOCTL_SETUPPER _IO(239,1)

int main(int argc, char **argv)
{
    int i,fd;
    char opt,write_buff[100],read_buff[100];

    fd = open(DEVICE,O_RDWR);

    if(fd == -1)
    {
        printf("file %s either does not exist or been locked by other process\n",DEVICE);

        exit(-1);
    }
    printf("Select : r -> read from device,w -> write to device, i -> for ioctl operation\n");
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
            if (fd< 0)
            {
                perror("Failed to read fd.\n");
                close(fd);
            }
            else
            {
                if(read_buff == NULL)
                {
                     printf("Buffer have no data\n");
                }

                printf("device: %s\n", read_buff);
            }
            break;
        
        case 'i':
            printf("IOCTL Operation\n");
            
            switch(atoi(argv[1]))
            {
                case 0:
                if (ioctl(fd,IOCTL_SETMIXED,0) < 0)
                {
                    perror("IOCTL operation Error.\n");
                    close(fd);
                }
                break;

                case 1:
                if (ioctl(fd,IOCTL_SETUPPER,0) < 0)
                {
                    perror("IOCTL operation Error.\n");
                    close(fd);
                }
                break;
                
                default:
                printf("Invalid argument");
                break;

            }
            break;

    

        default:
            printf("inavlid option\n");
            break;
    }
    close(fd);
    return 0;
}