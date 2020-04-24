#!/usr/bin/python
import os

print("starting receive [press Ctrl+C to exit]...\n")
try:
        f = os.open("/dev/uxmhfpvduart",os.O_RDWR)
        while True:
            ch = os.read(f,1)
            if ch != 0x00:
                sys.stdout.write(ch.decode())
                sys.stdout.flush()

except KeyboardInterrupt:
        os.close(f)
        print("Ctrl+C pressed, exiting!\n")
except IOError as e:
        print(e.errno)
        print(e)
