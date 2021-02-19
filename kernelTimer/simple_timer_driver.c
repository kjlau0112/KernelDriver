#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
 
static int __init timer_driver_init(void);
static void __exit timer_driver_exit(void);

//Timer Variable
#define TIMEOUT 5000    //milliseconds
static struct timer_list etx_timer;
static unsigned int count = 0;


//Timer Callback function. This will be called when timer expires
void timer_callback(struct timer_list * data)
{
     /* do your timer stuff here */
    printk(KERN_INFO "POC Timer Callback function Called [%d]\n",count++);
    
    /*
       Re-enable timer. Because this function will be called only first time. 
       If we re-enable this will work like periodic timer. 
    */
    mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}
 

static int __init timer_driver_init(void)
{
    // /*Allocating Major number*/
    // if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
    //         printk(KERN_INFO "Cannot allocate major number\n");
    //         return -1;
    // }
    // printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
    // /*Creating cdev structure*/
    // cdev_init(&etx_cdev,&fops);
 
    // /*Adding character device to the system*/
    // if((cdev_add(&etx_cdev,dev,1)) < 0){
    //     printk(KERN_INFO "Cannot add the device to the system\n");
    //     goto r_class;
    // }
 
    // /*Creating struct class*/
    // if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
    //     printk(KERN_INFO "Cannot create the struct class\n");
    //     goto r_class;
    // }
 
    // /*Creating device*/
    // if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
    //     printk(KERN_INFO "Cannot create the Device 1\n");
    //     goto r_device;
    // }
 
    /* setup your timer to call my_timer_callback */
    timer_setup(&etx_timer, timer_callback, 0);       //If you face some issues and using older kernel version, then you can try setup_timer API(Change Callback function's argument to unsingned long instead of struct timer_list *.
 
    /* setup timer interval to based on TIMEOUT Macro */
    mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));

    
 
    printk(KERN_INFO "Device Driver Insert...Done!!!\n");
    return 0;

}

static void __exit timer_driver_exit(void)
{
    /* remove kernel timer when unloading module */
    del_timer(&etx_timer);
 
    // class_destroy(dev_class);
    // cdev_del(&etx_cdev);
    // unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}


module_init(timer_driver_init);
module_exit(timer_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel timer POC");

