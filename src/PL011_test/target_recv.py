#!/usr/bin/python
import os
try:
    f = os.open("/dev/uxmhfpvduart",os.O_RDWR)
    #os.write(f,"test")
    data = os.read(f,18)
    print data
    os.close(f)
except IOError as e:
    print(e.errno)
    print(e)