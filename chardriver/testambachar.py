#!/usr/bin/python
import os
try:
    f = os.open("/dev/ambachar",os.O_RDWR)
    os.write(f,"test")
    data = os.read(f,16)
    print data
    os.close(f)
except IOError as e:
    print(e.errno) 
    print(e) 
