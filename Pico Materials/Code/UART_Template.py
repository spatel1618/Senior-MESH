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