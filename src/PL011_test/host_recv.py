import time
import serial
import sys

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate = 115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    rtscts=True,
    timeout=45
)

def do_read_serial () :
        ch= ser.read()
        sys.stdout.write(ch.decode())
        sys.stdout.flush()

        return;


print("starting logging [press Ctrl+C to abort]...\n")
try:
        while True:
                do_read_serial()
except KeyboardInterrupt:
        print("Ctrl+C pressed, exiting!\n")