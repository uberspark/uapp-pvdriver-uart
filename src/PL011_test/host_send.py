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

def do_send_serial () :

        print "{0}".format(ser.cts)
        ser.write ( "1".encode())
        print "{0}".format(ser.cts)
        ser.write ( "2".encode())
        print "{0}".format(ser.cts)
        ser.write ( "3".encode())
        print "{0}".format(ser.cts)
        ser.write ( "4".encode())
        print "{0}".format(ser.cts)
        ser.write ( "5".encode())
        print "{0}".format(ser.cts)
        ser.write ( "6".encode())
        print "{0}".format(ser.cts)
        ser.write ( "7".encode())
        print "{0}".format(ser.cts)
        ser.write ( "8".encode())
        print "{0}".format(ser.cts)
        ser.write ( "9".encode())
        print "{0}".format(ser.cts)
        ser.write ( "a".encode())
        print "{0}".format(ser.cts)
        #ser.write ( "b".encode())
        #print "{0}".format(ser.cts)
        #ser.write ( "c".encode())
        #print "{0}".format(ser.cts)
        #ser.write ( "d".encode())
        #print "{0}".format(ser.cts)
        #ser.write ( "e".encode())
        #print "{0}".format(ser.cts)
        #ser.write ( "f".encode())
        #print "{0}".format(ser.cts)
        #ser.write ( "g".encode())
        #print "{0}".format(ser.cts)
        #ser.write ( "h".encode())
        #print "{0}".format(ser.cts)
        #ser.write ( "i".encode())

        #ser.write(open("sample1.txt", "rb").read())

        return;


print("starting send...\n")
do_send_serial()
print("done sending!\n")