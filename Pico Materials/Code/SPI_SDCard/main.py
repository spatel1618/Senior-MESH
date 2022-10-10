''' Description: This file does the following:
                -Initializes SPI between a Raspberry Pi Pico and Adafruit MicroSD Card Breakout Board
                -Has the Raspberry Pi Pico write to the SD card
    Reference: https://learn.adafruit.com/adafruit-micro-sd-breakout-board-card-tutorial/circuitpython '''

import board
import busio
import sdcardio
import storage

''' To see all the available board-specific objects and pins for your board,
    enter the REPL (>>>) and run the following commands:
    import board
    dir(board) '''

# Use the board's primary SPI bus
spi = board.SPI()
# Or, use an SPI bus on specific pins:
#spi = busio.SPI(board.SD_SCK, MOSI=board.SD_MOSI, MISO=board.SD_MISO)

# For breakout boards, you can choose any GPIO pin that's convenient:
cs = board.D10
# Boards with built in SPI SD card slots will generally have a
# pin called SD_CS:
#cs = board.SD_CS

sdcard = sdcardio.SDCard(spi, cs)
vfs = storage.VfsFat(sdcard)

''' Finally you can mount the microSD card's filesystem into the CircuitPython filesystem.
    For example, to make the path /sd on the CircuitPython filesystem read and write from the card
    run this command: '''

storage.mount(vfs, "/sd")

''' At this point, you can read and write to the SD card using common Python functions like open, read, and write.
    The filenames will all begin with "/sd/" to differentiate them from the files on the CIRCUITPY drive. '''

# Create a file on the microSD card and write to it
with open("/sd/test.txt", "w") as f:
    f.write("Hello world!\r\n")
    
# Append a line to the file
with open("/sd/test.txt", "a") as f:
    f.write("This is another line!\r\n")
