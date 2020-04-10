import time
import serial
ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    rtscts=True,
    timeout=10
)
print ("rts state: " + str(ser.rts))
print ("cts state: " + str(ser.cts))
ser.write('I')
ser.write(' ')
ser.write('a')
ser.write('m')
ser.write(' ')
ser.write('t')
ser.write('w')
ser.write('e')
ser.write('n')
ser.write('t')
ser.write('y')
ser.write('-')
ser.write('f')
ser.write('o')
ser.write('u')
ser.write('r')
ser.write(' ')
ser.write('c')
ser.write('h')
ser.write('a')
ser.write('r')
ser.write('s')
ser.write('.')
ser.write('\n')
print ("rts state: " + str(ser.rts))
print ("cts state: " + str(ser.cts))