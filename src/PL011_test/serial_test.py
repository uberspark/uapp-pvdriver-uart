import time
import serial

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate = 115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    rtscts=True,
    timeout=10
)

readline = lambda : iter(lambda:ser.read(1),'\n')
while "".join(readline()) != '<<SENDFILE>>':
    pass
ser.write(open("test_file.txt", "rb").read())
ser.write('\n<<EOF>>\n')
