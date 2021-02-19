#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> 			// file operation structure
#include <linux/cdev.h>			// this is char driver
#include <linux/semaphore.h>	// sempahores header
#include <asm/uaccess.h>
#include <linux/uaccess.h>  	//API such as copy_to_user 
#include <linux/ioctl.h>
#define MAJOR_NUM 239
#define IOCTL_SETMIXED _IO(MAJOR_NUM,0)
#define IOCTL_SETUPPER _IO(MAJOR_NUM,1)
static int mode ;
//Introduce fake device
struct kj_device
{
	char data[100];
	struct semaphore sem;
}virtual_device;

struct cdev *mcdev; //creat new character device
int major_number;   // hold major number return  dev_t
int ret;
dev_t dev_num;		//hold major number that kernel provide

#define DEVICE_NAME "kjDevice"

// TODO: adding IOCTL feature in progressed
// This function triggered when system call
// on application  ioctl() is called
long hello_ioctl(struct file* flip, unsigned int ioctNum, unsigned long ioctl_param)
{
	switch(ioctNum)
	{
		case IOCTL_SETMIXED:
		mode =0;
		break;
		case IOCTL_SETUPPER:
		mode =1;
		break;

		default:
		mode = -1;
		break;
	}
	printk(KERN_INFO "IOCTL mode %d\n",mode);
	return 0;
}

// This function triggered when system call
// on application  open() is called
int device_open(struct inode *inode, struct file* file)
{

	/*
		* down_interruptible - acquire the semaphore unless interrupted
		* @sem: the semaphore to be acquired
		*
		* Attempts to acquire the semaphore.  If no more tasks are allowed to
		* acquire the semaphore, calling this function will put the task to sleep.
		* If the sleep is interrupted by a signal, this function will return -EINTR.
		* If the semaphore is successfully acquired, this function returns 0.
	*/
	if(down_interruptible(&virtual_device.sem)!=0)
	{
		printk(KERN_ALERT "kj_device ERR,could not lock device!");
		return -1;
	}
	printk(KERN_INFO "kj_device: device open");
	return 0;
}

// This function triggered when system call
// on application  read() is called
ssize_t device_read(struct file* filp,char* bufStoreData,size_t bufCount, loff_t *curOffset)
{
		//extract from user space to kernel
		printk(KERN_INFO "kj_device:Reading from device");

		if(bufStoreData !=NULL)
		{
			printk(KERN_INFO "kj_device:bufStoreData is NULL\n");
		}
		else 
		{
			printk(KERN_ALERT "kj_device:bufStoreData is not NULL\n");
		}

		ret = copy_to_user(bufStoreData,virtual_device.data,bufCount);
		return ret;
}


// This function triggered when system call
// on application  write() is called
ssize_t device_write(struct file* filp,const char* bufSourceData,size_t bufCount, loff_t *curOffset)
{
		//write from user space to kernel
		printk(KERN_INFO "kj_device:Writing to device");
		ret = copy_from_user(virtual_device.data,bufSourceData,bufCount);
		return ret;
}

int device_close(struct inode *inode, struct file *filp)
{
	up(&virtual_device.sem);
	printk(KERN_INFO "kj_device:closed device");
	return 0;
}

struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.write = device_write,
	.read = device_read,
	.unlocked_ioctl =hello_ioctl
};

//This function register character device dynamically.
static int driver_entry(void)
{
	//Allocates a range of char device numbers. The major number will be chosen dynamically, 
	//and returned (along with the first minor number) in dev. 
	//Returns zero or a negative error code.
	ret = alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
	if(ret<0)
	{
		printk(KERN_ALERT "kj_device ERR,Failed to allocate a major number!");
		return ret;
	}

	major_number = MAJOR(dev_num);  // Extract Major number and return
	printk(KERN_INFO "kj_device INFO,major number is %d\n",major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file",DEVICE_NAME,major_number);

	mcdev = cdev_alloc(); 			  // allocate kernel memory for my own structure
	mcdev-> ops = &fops;  			  // define your device driver file operation
	mcdev-> owner = THIS_MODULE;
	ret = cdev_add(mcdev,dev_num,1);  //device driver defined, added to the kernel
   	
	if(ret<0)
	{
		printk(KERN_ALERT "kj_device ERR,Unable to add cdev to kernel");
		return ret;
	}

	sema_init(&virtual_device.sem,1); //initialize semaphore as 1

	return 0;
}

//Unregisted everything in Reverse order
static void driver_exit(void)
{
	cdev_del(mcdev);
	unregister_chrdev_region(dev_num,1);
	printk(KERN_INFO "kj_device UNLOAD module");
		
}


module_init(driver_entry);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
