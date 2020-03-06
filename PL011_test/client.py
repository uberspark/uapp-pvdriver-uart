import time
import serial
ser = serial.Serial(
    port='/dev/ttyAMA0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    rtscts=True,
    timeout=10
)
ser.write('<<SENDFILE>>\n')
readline = lambda : iter(lambda:ser.read(1), "\n")
with open("sent_file.txt","wb") as outfile:
    while True:
        line = "".join(readline())
        if line == '<<EOF>>':
            break
        print >> outfile, line


