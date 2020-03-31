# PL011 Test Notes

## BLUF
I have completed my tests of the serial interface on the Raspberry Pi. Overall, the Pi demonstrated a reliable serial interface for the transmission of data, when using an on-board serial UART with hardware flow control.

## Test parameters

- PL011 UART on the Pi
- USB to UART FTDI cable
- Latest Raspbian image on Pi 3B+
- Ubuntu 19.10 Linux host on a X86_64 laptop
- Custom Python scripts to demonstrate a client and a server, utilizing the serial UART

Using this configuration, I was able to transfer a 26MB file from my Ubuntu host to the Pi in just over 41 minutes. From my back of the napkin calculation, I expected this transfer to take just over 30 minutes, transferring the data at 115,200bps. I attribute the extra transfer time to the slow write speed of the Pi 3B+ to its storage device, the SD Card. Basically, the two systems would transfer data until a buffer filled up, halt further transmission, flush the buffer to disk, then resume transmission. You could see this activity in real-time, as the link indicator was a solid red, interspersed with brief moments where no data was being sent at all. The hashes matched identically from the sent file and the received file, so the interface worked as expected.

Some minor tweaks to the Raspbian install were necessary to get the serial interface configured. First, there are two serial UARTs on the Pi, referred to as the "Mini UART" and the "PL011". The Mini UART is the default interface for the serial port and has some limitations that make it unsuitable for our test case. For example, the Mini UART does not support hardware flow control. To address this, we must configure the Pi to use the PL011 interface. In its default configuration, the Pi uses the PL011 UART to host the Bluetooth controller, so the Bluetooth controller and the Bluetooth service have to be disabled, before the UART is freed up for other purposes. This is accomplished with the following changes:

- In raspi-config, enable the serial UART, disabling the default terminal interface on the port.
- Using systemctl, disable/stop the hciuart service : sudo systemctl disable hciuart 
- In /boot/config.txt, apply configuration in a separate line:

```
dtoverlay=pi3-disable-bt
```

It turns out there are actually two options on some systems, disable-bt and pi3-disable-bt. You must use pi3-disable-bt in this case or it will not work. Your serial port will not connect.

Install the rpirtscts utility as described on this page:
https://github.com/mholling/rpirtscts


The PL011 uses a separate device file from the Mini UART and is found at /dev/ttyAMA0. Two additional tweaks are necessary to enable hardware flow control on the PL011:

- `stty -F /dev/ttyAMA0 crtscts`
- `rpirtscts on`

The first command sets terminal line settings for the device within the kernel. The second command is a custom script that was written for the Pi and was made available as a Github project. This command actually re-configures the Pi's GPIO pins for hardware flow control on the serial port. Its function is very simple and is a small amount of C code to do this.

With these configurations in place, we put one Python script on the host (server) that will transfer a file upon connection from a client and a python script on the client to initiate the transfer and save the file to persistent storage. The actual Python scripts could not be simpler. The scripts are located in this directory.

The set up of the client and server serial ports is identical, using the same Python serial port library. The only difference is in the behavior of the client and server.

After the test ran, I did a hash sum on the source and destination files and they were identical, so the transfer completed without error. Additionally, to validate that the hardware flow control was being used during this test, I ran an additional test and removed one of the GPIO pins from the hardware flow control path on the Pi. The result was that the transfer stalled. No further data was sent to the Pi after removing the flow control pin. I placed the connector back onto the pin and the transfer resumed. It was a nice surprise to see how resilient this mechanism was and did not cause any data errors.

## Character driver and user space test application 

### Driver ambachar.c

The driver ambachar is a new character device file interface and driver, useful for creating a userspace polling interface to the AMBA PL011 UART driver. It is modeled after the kgdb scheme and uses amba-pl011.c as a low level driver by calling the polling functions. This was verified by printk statements.  

The driver ambachar.c is placed in:

```
linux/drivers/tty/serial
```

the Makefile in this folder is used to replace the original Makefile, also in the linux/drivers/tty/serial folder. Recompile the modules with the command:   

```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j 16   
```

Transfer the new driver to the Pi:
```
scp ambachar.ko pi@ip_address_of_pi:~
```

Load the driver on the Pi:
```
ssh pi@ip_address_of_pi
insmod ambachar.ko
```

Check that the driver is loaded:
```
lsmod | grep ambachar
```

To unload the driver:
```
rmmod ambachar.ko
```
  
### Test Application  
  
The test application, testambachar.c, is an example of how the driver can be used. It can be built at the Raspberry Pi through Makefile.rpi. Transfer to Pi and put it in a separate folder along with Makefile.rpi. Rename the Makefile and run it:

```
mv Makefile.rpi Makefile
make  
sudo ./testambachar  
```

### Driver amba-pl011.c   
This is a file that merged the functionality of the original amba-pl011.c and ambachar.c. In this scenario there is no need of ambachar.c  

```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j 16   
scp amba-pl011.ko pi@ip_address_of_pi:~   // Transfer to the Pi    
rmmod amba-pl011.ko                       // Unload the driver     
insmod amba-pl011.ko                      // Load the driver    
lsmod | grep amba-pl011                   // Check that it is loaded     
```
