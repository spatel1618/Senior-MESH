from machine import Pin, ADC
from utime import sleep

# Initialize ADC pins
adc0 = ADC(26)
adc1 = ADC(27)
adc2 = ADC(28)

while(1):
    
    # Perform ADC and convert the digital value into a voltage reading
    
    ''' Note: The Raspberry Pi Pico has a 12-bit 3.3V ADC, but read_u16
              scales the 12-bit resolution to 16-bit resolution and
              hence enables readings from 0-65535 rather than 0-4096. '''
    
    adc0_value = adc0.read_u16() * 3.3 / 65536
    adc1_value = adc1.read_u16() * 3.3 / 65536
    adc2_value = adc2.read_u16() * 3.3 / 65536
    sleep(1) # X = Takes #seconds as input