# Character driver and user space test application 

## Driver

The driver ambachar is a new character device file interface and driver, useful
for creating a userspace polling interface to the AMBA PL011 UART driver.

TODO: Interface the char device to the PL011 UART.

The driver ambachar.c is placed in linux/drivers/tty/serial folder and   
the Makefile in this folder is used to replace the original Makefile.    
Recompile the modules with the command:   
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j 16   
scp ambachar.ko pi@ip_address_of_pi:~   // Transfer to the Pi   
insmod ambachar.ko                      // Load the driver   
lsmod | grep ambachar                   // Check that it is loaded   
rmmod ambachar.ko                       // Unload the driver    
  
## Test Application
  
The test application testambachar.c is an example of how the driver can be used.  
It can be built at the Raspberry Pi through Makefile.rpi.  
Transfer to Pi and put it in a separate folder along with Makefile.rpi.   
Rename Makefile.rpi to Makefile and run make.  
./testambachar  
