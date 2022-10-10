''' Description: This script does the following:
                -Sends a string to a Digi Zigbee 3 Digimesh 2.4 Firmware 300D via UART0 (Tx = GP0, Rx = GP1)
                -Repeats this process over a specified time interval
                
    UART References:
    - https://docs.micropython.org/en/latest/rp2/quickref.html
    - https://docs.micropython.org/en/latest/library/machine.UART.html#machine-uart '''

from machine import Pin, UART
from utime import sleep

# Initialize UART0
uart0 = UART(0, 9600)
uart0.init(baudrate=9600, bits=8, parity=None, stop=1, tx=Pin(0), rx=Pin(1), cts = Pin(2), rts = Pin(3))

# Set a sleep time in seconds for data collecting
sleepTime = 2
a = 1

while(59):
        
    # Send data to a XBee via UART
    uart0.write(str(a))
    
    # Put the system to sleep
    sleep(sleepTime) # X = Takes #seconds as input
    
    a = a + 1

'''
from machine import UART, Pin

#initialize
uart1 = UART(1, baudrate=9600, tx=Pin(4), rx=Pin(5))

#write
uart1.write(b'UART on GPIO4 & GPIO5 at 9600 baud\n\r')

#read
rxData = bytes()
while uart0.any() > 0:
    rxData += uart0.read(1)

print(rxData)
'''