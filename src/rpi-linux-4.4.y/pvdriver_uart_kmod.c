/*
 * @UBERXMHF_LICENSE_HEADER_START@
 *
 * uber eXtensible Micro-Hypervisor Framework (Raspberry Pi)
 *
 * Copyright 2018 Carnegie Mellon University. All Rights Reserved.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF
 * THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF
 * ANY KIND WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT
 * INFRINGEMENT.
 *
 * Released under a BSD (SEI)-style license, please see LICENSE or
 * contact permission@sei.cmu.edu for full terms.
 *
 * [DISTRIBUTION STATEMENT A] This material has been approved for public
 * release and unlimited distribution.  Please see Copyright notice for
 * non-US Government use and distribution.
 *
 * Carnegie Mellon is registered in the U.S. Patent and Trademark Office by
 * Carnegie Mellon University.
 *
 * @UBERXMHF_LICENSE_HEADER_END@
 */

/*
 * Author: Amit Vasudevan (amitvasudevan@acm.org)
 *
 */

#include <linux/init.h>           // macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // core header for loading LKMs into the kernel
#include <linux/device.h>         // header to support the kernel Driver Model
#include <linux/kernel.h>         // contains types, macros, functions for the kernel
#include <linux/fs.h>             // header for the Linux file system support
#include <linux/delay.h>
#include <linux/slab.h>
#include <asm/uaccess.h>          // required for the copy to user function

#include <uapp-pvdriver-uart.h>
#include "pvdriver_uart.h"

#define  DEVICE_NAME "uxmhfpvduart"    			//device will appear at /dev/uxmhfpvduart
#define  CLASS_NAME  "uxmhfpvduartchar"     	   //we are a character device driver

MODULE_LICENSE("GPL");				//appease the kernel
MODULE_AUTHOR("Amit Vasudevan");
MODULE_DESCRIPTION("Para-virtualized UART driver kernel-module char driver for uxmhf-rpi3");
MODULE_VERSION("0.1");

static int    major_number;
static int    number_opens = 0;
static struct class*  uxmhfpvduartcharClass  = NULL;
static struct device* uxmhfpvduartcharDevice = NULL;



//////
//prototypes for character driver interaction
//////
static int     dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int     dev_release(struct inode *, struct file *);

//////
//file operations structure to interface with the above
//////
static struct file_operations fops =
{
   .open = dev_open,
   .write = dev_write,
   .read = dev_read,	
   .release = dev_release,
};

static int dev_open(struct inode *inodep, struct file *filep){
   number_opens++;
   printk(KERN_INFO "uxmhfpvduartkmod: device has been opened %d time(s)\n", number_opens);
   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	switch(len){
		case UAPP_PVDRIVER_UART_UHCALL_INIT:
			printk(KERN_INFO "uxmhfpvduartkmod: initialize\n");
			uxmhfpvduart_init();
			printk(KERN_INFO "uxmhfpvduartkmod: initialization successful!\n");
			break;

		case UAPP_PVDRIVER_UART_UHCALL_SEND:
			printk(KERN_INFO "uxmhfpvduartkmod: send\n");
			if(uxmhfpvduart_send("Test", 4))
				printk(KERN_INFO "uxmhfpvduartkmod: send successful!\n");
			else
				printk(KERN_INFO "uxmhfpvduartkmod: error in send\n");
			break;

		case UAPP_PVDRIVER_UART_UHCALL_RECV:
		{
			bool readbufferexhausted;
			u32 len_read;
			u8 buffer[3];
			printk(KERN_INFO "uxmhfpvduartkmod: recv\n");
			memset(buffer, sizeof(buffer), 0);
			if(uxmhfpvduart_recv((u8  *)&buffer, sizeof(buffer), &len_read, &readbufferexhausted)){
				printk(KERN_INFO "uxmhfpvduartkmod: read: <0x%02x|0x%02x|0x%02x>, len_read=%u, \
						readbufferexhausted=%u\n", buffer[0], buffer[1], buffer[2],
						len_read, readbufferexhausted);
			}else{
				printk(KERN_INFO "uxmhfpvduartkmod: no data in recv buffer\n");
			}
		}
		break;

		case UAPP_PVDRIVER_UART_UHCALL_CAN_SEND:
			printk(KERN_INFO "uxmhfpvduartkmod: can send\n");
			if(uxmhfpvduart_can_send())
				printk(KERN_INFO "uxmhfpvduartkmod: transmit buffer has space!\n");
			else
				printk(KERN_INFO "uxmhfpvduartkmod: transmit buffer is full\n");
			break;

		case UAPP_PVDRIVER_UART_UHCALL_CAN_RECV:
			printk(KERN_INFO "uxmhfpvduartkmod: can recv\n");
			if(uxmhfpvduart_can_recv())
				printk(KERN_INFO "uxmhfpvduartkmod: recv buffer has data!\n");
			else
				printk(KERN_INFO "uxmhfpvduartkmod: no data in recv buffer\n");
			break;

		case UAPP_PVDRIVER_UART_UHCALL_FLUSH:
			printk(KERN_INFO "uxmhfpvduartkmod: going to flush UART buffers\n");
			uxmhfpvduart_flush();
			printk(KERN_INFO "uxmhfpvduartkmod: UART buffers flushed\n");
			break;

		default:
			printk(KERN_INFO "uxmhfpvduartkmod: unknown function, ignoring\n");
			break;
	}

	return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int l_error=0;
	u32 len_read;
	u8 *l_buffer;
	bool readbufferexhausted;

	printk(KERN_INFO "uxmhfpvduartkmod: recv\n");
	
	//allocate buffer
	l_buffer = kmalloc(4096, GFP_KERNEL);
	if(l_buffer){

		if(uxmhfpvduart_recv((u8  *)l_buffer, sizeof(l_buffer), &len_read, &readbufferexhausted)){
			//successful invocation to recv, copy to buffer if there is somethins read
			if(len_read){
				printk(KERN_INFO "uxmhfpvduartkmod: received %u bytes, copying to user buffer\n", len_read);
				l_error = copy_to_user(buffer, (char *)&l_buffer, len_read);
			}else{
				printk(KERN_INFO "uxmhfpvduartkmod: nothing to receive\n");
				l_error=0;
			}

			if(l_error != 0)
				l_error = -EFAULT;

		}else{
			//error in recv
			printk(KERN_INFO "uxmhfpvduartkmod: error in recv!\n");
			l_error = -EFAULT;
		}

		//free the recv buffer
		kfree(l_buffer);

	}else{
		//error in buffer allocation
		printk(KERN_INFO "uxmhfpvduartkmod: error in allocating recv buffer!\n");
		l_error=-EFAULT;
	}


	printk(KERN_INFO "uxmhfpvduartkmod: l_error=%u\n", l_error);
	return l_error;
}



static int dev_release(struct inode *inodep, struct file *filep){
   number_opens--;
   printk(KERN_INFO "uxmhfpvduartkmod: device successfully closed\n");
   return 0;
}




//////
//module initialization function
//////
int uxmhfpvduartkmod_init(void)
{

	printk(KERN_INFO "uxmhfpvduartkmod: LOAD\n");
	printk(KERN_INFO "author: amit vasudevan (amitvasudevan@acm.org)\n");

	//try to allocate a major number dynamically
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number<0){
	  printk(KERN_ALERT "uxmhfpvduartkmod: failed to register a major number\n");
	  return major_number;
	}
	printk(KERN_INFO "uxmhfpvduartkmod: registered correctly with major number %d\n", major_number);

	// Register the device class
	uxmhfpvduartcharClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(uxmhfpvduartcharClass)){
	  unregister_chrdev(major_number, DEVICE_NAME);
	  printk(KERN_ALERT "uxmhfpvduartkmod: Failed to register device class\n");
	  return PTR_ERR(uxmhfpvduartcharClass);
	}
	printk(KERN_INFO "uxmhfpvduartkmod: device class registered correctly\n");

	// register the device driver
	uxmhfpvduartcharDevice = device_create(uxmhfpvduartcharClass, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(uxmhfpvduartcharDevice)){
	  class_destroy(uxmhfpvduartcharClass);
	  unregister_chrdev(major_number, DEVICE_NAME);
	  printk(KERN_ALERT "uxmhfpvduartkmod:Failed to create the device\n");
	  return PTR_ERR(uxmhfpvduartcharDevice);
	}
	printk(KERN_INFO "uxmhfpvduartkmod: device class created correctly\n");


	return 0;
}



//////
//module unload function
//////
void uxmhfpvduartkmod_exit(void)
{
	device_destroy(uxmhfpvduartcharClass, MKDEV(major_number, 0));     // remove the device
	class_unregister(uxmhfpvduartcharClass);                          // unregister the device class
	class_destroy(uxmhfpvduartcharClass);                             // remove the device class
	unregister_chrdev(major_number, DEVICE_NAME);             // unregister the major number
	printk(KERN_INFO "uxmhfpvduartkmod: UNLOAD\n");
}

module_init(uxmhfpvduartkmod_init);
module_exit(uxmhfpvduartkmod_exit);


//////
// functionality that should eventually be exposed via ioctl
//////
#if 0



#endif