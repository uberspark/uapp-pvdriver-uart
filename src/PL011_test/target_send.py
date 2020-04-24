#!/usr/bin/python
import os
try:
    f = os.open("/dev/uxmhfpvduart",os.O_RDWR)
    os.write(f,"hello world from UART!")
    os.close(f)
except IOError as e:
    print(e.errno)
    print(e)