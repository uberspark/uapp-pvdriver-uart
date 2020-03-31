/* File   ambachar.c
 * @brief   An introductory character driver to support the second article of my series on
 * Linux loadable kernel module (LKM) development. This module maps to /dev/ambachar and
 * comes with a helper C program that can be run in Linux user space to communicate with
 * this the LKM.
 */
 
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/ctype.h>
#include <linux/kgdb.h>
#include <linux/kdb.h>
#include <linux/tty.h>
#include <linux/console.h>
#include <linux/vt_kern.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/kgdb.h>
#include <linux/kdb.h>
#include <linux/tty.h>
#include <linux/console.h>
#include <linux/vt_kern.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/serial_core.h>



#define  DEVICE_NAME "ambachar"    ///< The device will appear at /dev/ambachar using this value
#define  CLASS_NAME  "amba"        ///< The device class -- this is a character device driver
#define MAX_CONFIG_LEN		40
static char config[MAX_CONFIG_LEN] = "ttyAMA0";
static struct kgdb_io		kgdboc_io_ops;
static struct tty_driver	*kgdb_tty_driver;
static int			kgdb_tty_line;
 
MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Derek Molloy");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for the BBB");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users
 
static int    majorNumber;                  ///< Stores the device number -- determined automatically
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  ambacharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ambacharDevice = NULL; ///< The device-driver device struct pointer
 
// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
 
/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};
 

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init ambachar_init(void){
   int tty_line = 0;
   char *cptr = config;
   struct tty_driver *p;
   struct console *cons;

   printk(KERN_INFO "AmbaChar: Initializing the AmbaChar LKM\n");
   kgdb_tty_driver = NULL;

   p = tty_find_polling_driver(cptr, &tty_line);

   if(p == NULL){
       printk(KERN_INFO "AmbaChar: Could not find polling driver.\n");
   }
   else{
       printk(KERN_INFO "AmbaChar: Found polling driver.\n");
   }

   cons = console_drivers;
   while (cons) {
         int idx;
         if (cons->device && cons->device(cons, &idx) == p &&
               idx == tty_line) {
                   kgdboc_io_ops.is_console = 1;
                   break;
          }
          cons = cons->next;
   }

   kgdb_tty_driver = p;
   kgdb_tty_line = tty_line;
   
   printk("kgdb_tty_driver is set to :  %p\n",kgdb_tty_driver);
   printk("kgdb_tty_line is set to :  %d\n",kgdb_tty_line);

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "AmbaChar failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "AmbaChar: registered correctly with major number %d\n", majorNumber);
 
   // Register the device class
   ambacharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ambacharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ambacharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "AmbaChar: device class registered correctly\n");
 
   // Register the device driver
   ambacharDevice = device_create(ambacharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ambacharDevice)){               // Clean up if there is an error
      class_destroy(ambacharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ambacharDevice);
   }
   printk(KERN_INFO "AmbaChar: device class created correctly\n"); // Made it! device was initialized
   return 0;
}
 
/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit ambachar_exit(void){
   device_destroy(ambacharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ambacharClass);                          // unregister the device class
   class_destroy(ambacharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "AmbaChar: Goodbye from the LKM!\n");
}
 
/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "AmbaChar: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
 
/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int i;
   int read_count = 0;
   int ret_char;
   printk("dev_read() called \n");
   for(i=0;i<len;i++){
      ret_char =kgdb_tty_driver->ops->poll_get_char(kgdb_tty_driver, kgdb_tty_line);
      if(ret_char != NO_POLL_CHAR){
	 buffer[i] = ret_char;
	 read_count++;
      }
      else{
	  break;
      }
   } 
   return read_count;
}
 
/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   int i;
   printk("dev_write() called \n");
   for(i=0;i<len;i++){
      kgdb_tty_driver->ops->poll_put_char(kgdb_tty_driver,kgdb_tty_line, buffer[i]);
   }

   return len;
}
 
/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "AmbaChar: Device successfully closed\n");
   return 0;
}
 
/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(ambachar_init);
module_exit(ambachar_exit);
