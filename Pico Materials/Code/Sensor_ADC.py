''' Description: This script does the following:
                -Collects Raspberry Pi Pico sensor data with 3 ADCs (Pins GP26-GP28)
                -Sends readable sensor data to a Digi Zigbee 3 via UART (Tx = GP4, Rx = GP5)
                -Repeats this process over a specified time interval
    
    ADC References:
    - https://how2electronics.com/how-to-use-adc-in-raspberry-pi-pico-adc-example-code/
    - https://microcontrollerslab.com/raspberry-pi-pico-adc-tutorial/
                
    UART References:
    - https://docs.micropython.org/en/latest/rp2/quickref.html
    - https://docs.micropython.org/en/latest/library/machine.UART.html#machine-uart '''

from machine import Pin, ADC, UART
from utime import sleep

''' Detach the UART from REPL mode
import os
os.dupterm(None, 1) '''

# Initialize ADC pins
adc0 = ADC(26)
adc1 = ADC(27)
adc2 = ADC(28)

# Initialize UART0
uart0 = UART(0, 9600)
uart0.init(baudrate=9600, bits=8, parity=None, stop=1, tx=Pin(0), rx=Pin(1), cts = Pin(2), rts = Pin(3), flow = UART.RTS | UART.CTS)

# Set a sleep time in seconds for data collecting
sleepTime = 2

while(59):
    
    # Perform ADC and convert the digital value into a voltage reading
    
    ''' Note: The Raspberry Pi Pico has a 12-bit 3.3V ADC, but read_u16
              scales the 12-bit resolution to 16-bit resolution and
              hence enables readings from 0-65535 rather than 0-4096. '''
    
    adc0_value = adc0.read_u16() * 3.3 / 65536
    adc1_value = adc1.read_u16() * 3.3 / 65536
    adc2_value = adc2.read_u16() * 3.3 / 65536
    
    # Send the formatted data to a XBee via UART
    #uart0.write('ADC0 Reading: ' + str(adc0_value))
    uart0.write("Hello\r\n")
    #uart0.write('ADC1 Reading: ' + str(adc1_value))
    #uart0.write('ADC2 Reading: ' + str(adc2_value))
    
    # Put the system to sleep
    sleep(sleepTime) # X = Takes #seconds as input
