""" Description: This script enables a XBee 3 Digimesh 2.4 Firmware 300D module
                 to receive UART transmissions from a Raspberry Pi Pico.

    References:
        https://www.digi.com/resources/documentation/digidocs/PDFs/90002219.pdf
        https://forums.raspberrypi.com/viewtopic.php?t=319345
"""
from sys import stdin
from sys import stdout
import xbee

# Initialize the XBee's UART
# Note that sys.stdin only works if ATAP = 4
xbee.atcmd("AP", 4)  # Micropython REPL mode
xbee.atcmd("PS", 1)  # Autostart Micropython code when the board powers up
xbee.atcmd("BD", 3)  # baud rate = 9600
xbee.atcmd("NB", 0)  # no parity
xbee.atcmd("SB", 0)  # 1 stop bit
xbee.atcmd("D6", 1)  # enable RTS
xbee.atcmd("D7", 1)  # enable CTS
xbee.atcmd("P3", 1)  # XBee SMT Grove pin DIO13 for TX
xbee.atcmd("P4", 1)  # Xbee SMT Grove pin DIO14 for RX

while True:
    # data = stdin.buffer.read()
    data = stdin.read()
    if data is not None:
        print(data)
    # stdout.buffer.write(data)
