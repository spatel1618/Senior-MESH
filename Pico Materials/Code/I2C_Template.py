from machine import Pin, I2C
from utime import sleep

# Initialize I2C0
i2c = machine.I2C(0, scl=machine.Pin(17), sda=machine.Pin(16), freq = 400000)

# Scan devices connected to the I2C pins and display their information
print('Scan i2c bus...')
devices = i2c.scan()

if len(devices) == 0:
print("No i2c device !")
else:
print('i2c devices found:',len(devices))

for device in devices:
print("Address: ",hex(device))

while(1):
    
    # Send
    sleep(1) # X = Takes #seconds as input
