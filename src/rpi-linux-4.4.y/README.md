Para-virtualized UART driver kernel module for Raspberry Pi 3
=============================================================

Requirements
------------

1. uberXMHF for Raspberry Pi 3 (https://github.com/uberspark/uberxmhf.git). Grab the latest develop branch.
2. Raspberry Pi 3 Linux Kernel ( https://github.com/raspberrypi/linux.git). Switch to branch rpi-4.4.y
3. Raspberry Pi 3 cross-compiler toolchain (https://github.com/raspberrypi/tools)
   a. use the arm-linux-gnueabihf- tools within arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/ in the above repository


Prepping Raspbian Linux on the Pi3
----------------------------------
1. ensure that the kernel command line for the pi3 __does not__ contain console=serialxxx options. this is to avoid
   potential issues with serial port multi-plexing from the Raspbian (guest) OS kernel and micro-hypervisor

2. boot the PI3 __without__ uberXMHF and issue the following two commands to deactivate the serial port driver for 
   the Raspbian (guest) OS
  1. `sudo systemctl stop serial-getty@serial0`
  1. `sudo systemctl disable serial-getty@serial0`
  1. `sudo systemctl stop serial-getty@serial1`
  1. `sudo systemctl disable serial-getty@serial1`



Building and Installing uberXMHF
--------------------------------

1. do a make docs_html within uberxmhf.git/docs folder and read uberxmhf.git/docs/_build/index.html. Refer to
   raspberry pi3 build and installation instructions. Note: you will need to pass --enable-uart-pl011 and --enable-uapp-pvdriver-uart at the build configuration step.


2. you will also need to have the debugging setup as mentioned in the raspberry pi3 debugging documentation within
   uberXMHF so you can test the UART para-virtualized driver with the test scripts. As per the documentation we
   will refere to the target system as the Pi3 running uberXMHF along with the UART para virtualized driver and the
   host system as the test-bed dual.
      


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

2. load the module using
   sudo insmod ~/uxmhf_pvduart_kmod.ko

3. perform test

4. unload the module using
   sudo rmmod ~/uxmhf_pvduart_kmod.ko

