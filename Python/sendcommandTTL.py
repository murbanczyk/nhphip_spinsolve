import serial
import io
import sys
import time
import datetime

port_location = 'COM3' 

baud_rate = 9600

sendstring = ""
receivestring = b"" # Receivestring has to be bytestring
for arg in sys.argv[1::]:
        sendstring += (str(arg)+" ")
    
with serial.Serial(port_location, baud_rate, timeout=1) as ser:
    # Do not forget to add '\n' character! for line termination
    ser.write((sendstring+'\r\n').encode())
    time.sleep(0.5)
    # Arduino reprints what was sent to the serial monitor
    if (ser.in_waiting > 0):
        #time.sleep(0.5)
        receivestring = b""
        while (ser.in_waiting > 0):
            receivestring += ser.read(size=1)
        receivestring = receivestring.decode("ascii")
        
        print (datetime.datetime.now().strftime("%d/%m/%Y, %H:%M:%S") + " ::: " +str(receivestring).strip('\r\n'))
    ser.flush()