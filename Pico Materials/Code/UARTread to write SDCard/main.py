import machine
from machine import Pin, UART
import sdcard
import uos

'''
Spi 1 Rpi pinout:
    SCK = GP10 (pin 14)
    Tx = GP11 (pin 15)
    Rx = GP12 (pin 16)
    CS = GP13 (pin 17)
'''

# Initialize UART0
uart0 = UART(0, 9600)
uart0.init(baudrate=9600, bits=8, parity=None, stop=1, tx=Pin(0), rx=Pin(1), cts = Pin(2), rts = Pin(3))


# Assign chip select (CS) pin (and start it high)
cs = machine.Pin(13, machine.Pin.OUT)

# Intialize SPI peripheral (start with 1 MHz)
spi = machine.SPI(1,
                  baudrate=1000000,
                  polarity=0,
                  phase=0,
                  bits=8,
                  firstbit=machine.SPI.MSB,
                  sck=machine.Pin(10),
                  mosi=machine.Pin(11),
                  miso=machine.Pin(12))

# Initialize SD card
sd = sdcard.SDCard(spi, cs)

# Mount filesystem
vfs = uos.VfsFat(sd)
uos.mount(vfs, "/sd")


while(1):
    data = uart0.read()
    if data is not None:
     
        # Create a file and write something to it
        with open("/sd/test01.txt", "w") as file:
            file.write(str(data) + "\r\n")
            
