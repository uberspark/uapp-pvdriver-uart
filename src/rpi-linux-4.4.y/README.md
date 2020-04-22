Para-virtualized UART driver kernel module for Raspberry Pi 3
=============================================================

Requirements
------------

1. uberXMHF for Raspberry Pi 3 (https://github.com/uberspark/uberxmhf.git). Grab the latest develop branch.
2. Raspberry Pi 3 Linux Kernel ( https://github.com/raspberrypi/linux.git). Switch to branch rpi-4.4.y
3. Raspberry Pi 3 cross-compiler toolchain (https://github.com/raspberrypi/tools)
   a. use the arm-linux-gnueabihf- tools within arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/ in the above repository


Building the kernel module
--------------------------

make -C $PATH_RPI_LINUX_44y  ARCH=arm CROSS_COMPILE=$PATH_RPI_CROSS_COMPILE_TOOLS M=$PWD PATH_UXMHF=$PATH_UXMHF_RPI3

Here,
$PATH_RPI_LINUX_44y is the absolute path to where you cloned the rasberry pi 3 linux kernel (e.g., /usr/local/rpi-linux.git)
$PATH_RPI_CROSS_COMPILE_TOOLS is the absolute path to raspberry pi 3 tools arm-linux-gnueabihf- tools directory 
 (e.g., /usr/local/rpi-tools.git/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-)
$PATH_UXMHF_RPI3 is the absolute path to where you cloned the uberXMHF source git repository (e.g., /usr/local/uberspark/uberxmhf.git/)


