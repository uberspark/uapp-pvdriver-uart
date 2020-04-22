# top-level makefile for building uart character para-virtualized driver
#
# author: amit vasudevan (amitvasudevan@acm.org)

# this makefile depends on the following external environment variable definitions
#
# PATH_UXMHF = full path to uxmhf (git) source tree
#

obj-m := uxmhfpvduartkmod.o

uxmhfpvduartkmod-objs := pvdriver_uart_kmod.o pvdriver_uart_kmodsup.o pvdriver_uart_kmodlib.o

INCS = -I$(PATH_UXMHF)/uxmhf-rpi3/include


	