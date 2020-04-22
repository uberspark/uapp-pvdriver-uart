# top-level makefile for building uart character para-virtualized driver
#
# author: amit vasudevan (amitvasudevan@acm.org)

# this makefile depends on the following external environment variable definitions
#
# PATH_UXMHF = full path to uxmhf (git) source tree
#
CURRENT_DIR := $(realpath $(src))


obj-m := uxmhf_pvduart_kmod.o
ccflags-y := -I$(CURRENT_DIR) -I$(PATH_UXMHF)/uxmhf-rpi3/include


uxmhf_pvduart_kmod-objs := pvdriver_uart_kmod.o pvdriver_uart_kmodsup.o pvdriver_uart_kmodlib.o




	