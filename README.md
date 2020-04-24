Para-virtualized UART driver kernel module for Raspberry Pi 3
=============================================================

Requirements
------------

1. uberXMHF for Raspberry Pi 3 (https://github.com/uberspark/uberxmhf.git). Grab the latest develop branch.
1. Raspberry Pi 3 Linux Kernel ( https://github.com/raspberrypi/linux.git). Switch to branch rpi-4.4.y
1. Raspberry Pi 3 cross-compiler toolchain (https://github.com/raspberrypi/tools)
   a. use the arm-linux-gnueabihf- tools within arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/ in the above repository


Prepping Raspbian Linux on the Pi3
----------------------------------
1. ensure that the kernel command line for the pi3 __does not__ contain console=serialxxx options. this is to avoid
   potential issues with serial port multi-plexing from the Raspbian (guest) OS kernel and micro-hypervisor

1. boot the PI3 __without__ uberXMHF and issue the following two commands to deactivate the serial port driver for 
   the Raspbian (guest) OS
  1. `sudo systemctl stop serial-getty@serial0`
  1. `sudo systemctl disable serial-getty@serial0`
  1. `sudo systemctl stop serial-getty@serial1`
  1. `sudo systemctl disable serial-getty@serial1`



Building and Installing uberXMHF
--------------------------------

1. do a `make clean` followed by a `make docs_html` within `uberxmhf.git/docs` folder and read 
   `uberxmhf.git/docs/_build/index.html`. Refer to
   raspberry pi3 build and installation instructions. Note: you will need to pass `--enable-uart-pl011`, 
   `--enable-uart-pl011-ctsrts`, and `--enable-uapp-pvdriver-uart` at the build configuration step.

   1. note: you __cannot__ specify ``--enable-debug-uart`` in the context above
   1. make sure to read and follow the additional requirements for kernel recompilation for --enable-uapp-pvdriver-uart
      during uberXMHF build and installation
   1. with the above options, we are limited to interacting with the PI3 either via a dedicated screen 
      and keyboard or via SSH (network). The serial interface will not be available for logging in and interacting
      with the pi3

1. you will also need to have the debugging setup as mentioned in the raspberry pi3 debugging documentation within
   uberXMHF so you can test the UART para-virtualized driver with the test scripts. As per the documentation we
   will refer to the `target system` as the Pi3 running uberXMHF along with the UART para virtualized driver and the
   `host system` as the test-bed dual.
      


For the steps below: 
$PATH_RPI_LINUX_44y is the absolute path to where you cloned the rasberry pi 3 linux kernel (e.g., /usr/local/rpi-linux.git)
$PATH_RPI_CROSS_COMPILE_TOOLS is the absolute path to raspberry pi 3 tools arm-linux-gnueabihf- tools directory 
 (e.g., /usr/local/rpi-tools.git/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-)
$PATH_UXMHF_RPI3 is the absolute path to where you cloned the uberXMHF source git repository (e.g., /usr/local/uberspark/uberxmhf.git/)


Building the kernel module
--------------------------

make -C $PATH_RPI_LINUX_44y  ARCH=arm CROSS_COMPILE=$PATH_RPI_CROSS_COMPILE_TOOLS M=$PWD PATH_UXMHF=$PATH_UXMHF_RPI3


Clean up
---------

make -C $PATH_RPI_LINUX_44y  ARCH=arm CROSS_COMPILE=$PATH_RPI_CROSS_COMPILE_TOOLS M=$PWD PATH_UXMHF=$PATH_UXMHF_RPI3 clean


Usage
-----

1. copy over uxmhf_pvduart_kmod.ko to the Pi3 (e.g., at ~/uxmhf_pvduart_kmod.ko)

1. copy over `PL011_test/target_recv.py` and `PL011_test/target_send.py` to the Pi3 (e.g., at ~/.)

1. boot the Pi3 with uberXMHF and once the guest OS has booted up, load the module using
   sudo insmod ~/uxmhf_pvduart_kmod.ko

1. run python PL011_test/host_recv.py on the host system and run python ~/target_send.py on the Pi3
   if everything goes well,  the host system should receive the string "hello world from UART!" successfully!

1. run python PL011_test/host_send.py on the host system and run python ~/target_recv.py on the Pi3
   if everything goes well,  the pi3 should receive the string "hello world from UART!" successfully!

1. unload the module using
   sudo rmmod uxmhf_pvduart_kmod.ko


Kernel-module Developer Notes
-----------------------------

This section is for developers intending to use the para-virtualized UART functionality from within their
__own__ kernel module.

1. copy over `pvdriver_uart_kmodlib.c` and `pvdriver_uart.h` to your own kernel module source directory and 
   add them to the `Makefile`

1. the following are functions are now available to the kernel-module developer:

	1. `void uxmhfpvduart_init(void)` : this function initializes the 
	UART hardware with 115200 baudrate with the typical `8N1` (8 data bits,
	no parity bit, 1 stop bit) configuration. 

	1. `bool uxmhfpvduart_send(u8 *buffer, u32 buf_len)` : this function
	sends (writes) a `buffer` of `buf_len` bytes to the UART. returns `true` on
	sucess and `false` on failure.

	1. `bool uxmhfpvduart_recv(u8 *buffer, u32 max_len, u32 *len_read, bool *uartreadbufexhausted)` :
	this function reads the UART receive buffer into the `buffer` specified 
	upto `max_len` bytes and returns the actual number of bytes read into 
	`len_read`. The function sets `uartreadbufexhausted` to `true` if the UART receive buffer is empty as a result	of this read. A return value of `false` in 
	`uartreadbufexhausted` indicates the UART receive
	buffer still has pending bytes to be read after this function returns 
	(e.g., if `max_len` was less than number of bytes in the UART receive
	buffer).

	1. `bool uxmhfpvduart_can_send(void)` : this function returns `true` if
	the UART is ready to transmit data. A return
	value of `false` indicates that the UART is not ready to transmit.

	1. `bool uxmhfpvduart_can_recv(void)` : this function returns `true` if
	there are bytes waiting to be read from the UART receive buffer. A return
	value of `false` signifies an empty receive buffer.

	1. `void uxmhfpvduart_flush(void)` : this function flushes the UART transmit
   and receive FIFO buffers.
