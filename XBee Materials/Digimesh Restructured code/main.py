                      ############## MESH NETWORK USING DIGI-MESH NETWORK SKELETON ##############


################################################### PROGRAM USERSPACE ################################################


"""DEPENDENCIES"""

import xbee
import time
from machine import Pin
from sys import stdin
from sys import stdout

"""CONSTANTS"""

NETWORK_AWAKE = 0xB
MAX_SLEEP_TIME_MINS = 240

"""GENERAL SETTINGS"""

#NETWORK CONFIGURATION SETTINGS
Channel = 0xC
PanID = 0x3737

#PIN CONFIGURATION
buttonPin = Pin(Pin.board.D4, Pin.IN, Pin.PULL_UP)
rledPin = Pin(Pin.board.D3, Pin.OUT, value=0)
gledPin = Pin(Pin.board.D5, Pin.OUT, value=0)
bledPin = Pin(Pin.board.D12, Pin.OUT, value=0)

########################################################################################

"""COMMAND FUNCTIONS"""
"""
DESCRIPTION: Command Functions that are called as a result of a received command
PASS : N/A
RETURN : RESPONSE (string) 
"""

#SAMPLE IF BUTTON IS PRESSED USING GPIO
def buttonCmd():
    response = ""
    if buttonPin.value() == 1:
        response += "X"
    else:
        response += "O"
    return response

def picoReadCmd():
    response = ""

    data = stdin.read()
    if data is not None:
        response += "%.2f" % data
    else:
        response = "No Sample"

    return response


def picoWriteCmd(response):
    stdout.buffer.write(str(response))


"""COMMUNICATION SETTINGS"""

#DEVICE LIST OF NODES FOR COORDINATOR TO ISSUE COMMANDS TO
Devs = []



#TYPES OF COMMANDS
gpioCmds = [picoReadCmd]
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DEFINE MORE HERE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#COMMANDS THE COORDINATOR WILL ISSUE
Cmds = {
    b'gpioRead': gpioCmds,
    #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DEFINE MORE HERE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
}



########################################################################################

################################################### PROGRAM USERSPACE ################################################












################################################### PROGRAM OPERATION ################################################

########################################################################################

"""NETWORK CALLBACK FUNCTIONS"""
"""
DESCRIPTION: Callbacks activate when the network wakes up after defined sleep time

PASS : modem_status (int)
RETURN : N/A 
"""
#Coordinator Callback
def coordCallBack(status):
    if status == NETWORK_AWAKE:
        #For all sensor nodes
        for dev in Devs:
            print("Device %s: " % (''.join('{:02x}'.format(x).upper() for x in dev)))

            #xbee.transmit(dev, tx_options=1)

            # Poll for response
            received_msg = xbee.receive()

            #while not received_msg:
            #    received_msg = xbee.receive()

            picoWriteCmd(received_msg)

            print("\t{} <- {}".format("picoWriteCmd", received_msg))

        print() #insert newline

#Sensor Callback
def sensorCallBack(status):
    if status == NETWORK_AWAKE:
        bledPin.on()

        response = "%.2f" % picoReadCmd()

        #Transmit Command Response to sunk Coordinator
        xbee.transmit(xbee.atcmd("DH") + xbee.atcmd("DL"), response, tx_options=1)

        print("{}".format(response))

    bledPin.off()

########################################################################################

"""SLEEP FUNCTIONS"""

"""
DESCRIPTION: Accepts user input to determine how long network should sleep

PASS : 
RETURN : formatted sleep time (int) or error
"""
def setNetworkSleepTime():
    sleepTime = input("\nHow often should the network wake up in MINUTES (Max = 240 minutes)?: ")

    value = -1
    # Error Checking
    try:
      value = int(sleepTime)
    except ValueError:
      try:
          # Convert it into float
          value = float(sleepTime)
      except ValueError:
          print("Error: Input is NaN ...")
          return -1

    if value < 0:
        print("Error: Enter a number from 0-240 ...")
        return -1

    if value > MAX_SLEEP_TIME_MINS:
        print("Error: Enter a number from 0-240 ...")
        return -1

    print("Sleep Time Set To: {} minutes\n".format(value))

    return int(value * 6000)
"""
DESCRIPTION: Modifies sleep time of a specific node. Can be used with sleep coordinator to set entire network

PASS : sleepTime (int)
RETURN : N/A 
"""
def changeSleep(sleepTime):
    xbee.atcmd("SP", sleepTime)
    xbee.atcmd("WR")

"""
DESCRIPTION: Sets a sensor to a synchronous sleep mode

PASS : N/A
RETURN : N/A 
"""
def sensorSleep():
    #Sleep configuration for a router
    xbee.atcmd("SM", 8)      #Synchronous sleep mode
    xbee.atcmd("SP", 1)      #Fast Sleep for syncing

    xbee.atcmd("SO", 0x4)    #Sleep for a router

    xbee.atcmd("WR")         # Save all settings to flash

"""
DESCRIPTION: Sets coordinator to a synchronous sleep mode

PASS : N/A
RETURN : N/A 
"""
def coordSleep():
    #Sleep configuration for a router
    xbee.atcmd("SM", 8)      #Synchronous sleep mode
    xbee.atcmd("SP", 1)      #Fast Sleep for syncing

    xbee.atcmd("SO", 0x5)    # Sleep coordinator

    xbee.atcmd("WR")         # Save all settings to flash

########################################################################################

"""BOOT FUNCTIONS"""

"""
DESCRIPTION: Boots coordinator by-
            1. Pairing to a Channel
            2. Discovering other devices on the network
            3. Sinking all routes to other devices on the network
PASS : N/A
RETURN : N/A 
"""
def cBoot():
    #Indicate booting a coordinator
    rledPin.on()

    #Connect to predetermined network
    xbee.atcmd("CH", Channel)
    xbee.atcmd("ID", PanID)
    xbee.atcmd("NI", "Data Sink")
    xbee.atcmd("CE", 3)              # Non - routing coordinator
    xbee.atcmd("AP", 4)              # Micropython mode
    xbee.atcmd("WR")                 # Save all settings to flash

    #List all devices vai broadcast message
    print("DISCOVERING DEVICES...\n")

    while True:
        count = 1
        for device in xbee.discover():
            addr64 = device['sender_eui64']
            node_id = device['node_id']
            rssi = device['rssi']

            print("Sensor %d:\n"
                  "  - 64-bit address: %s\n"
                  "  - Node identifier: %s\n"
                  "  - RSSI: %d\n"
                  % (count, ''.join('{:02x}'.format(x).upper() for x in addr64), node_id, rssi))

            Devs.append(addr64)
            count += 1

        #Determine if you need to ping all devices again
        discoverSuccess = input("Were all sensors discovered (y/n)?: ")
        if discoverSuccess == 'Y' or discoverSuccess == 'y':
            break
        else:
            print("RE-DISCOVERING DEVICES...\n")

    #Check to make sure correct number of devices is available
    if len(Devs) == 0:
        print("ERROR: No devices discovered... Please reset device...")
        while True:
            continue

    #Input the number of minutes to sleep
    Sleep_10Ms = -1
    while True:
        Sleep_10Ms = setNetworkSleepTime()
        if Sleep_10Ms > 0:
            break

    rledPin.off()

    #Sink Routes for continuing energy efficient uni-cast transmissions
    print("SINKING ROUTES...")
    xbee.atcmd("AG", bytes((0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF)))

    #Give sensor nodes sometime to configure
    time.sleep(1)

    return Sleep_10Ms          #Every 1 hour seconds

"""
DESCRIPTION: Boots router by-
            1. Pairing to a Channel
            2. Discovering other devices on the network
            3. Sinking all routes to other devices on the network
PASS : N/A
RETURN : N/A 
"""
def rBoot():
    #Indicate booting a router
    bledPin.on()

    #Connect to predetermined network
    xbee.atcmd("CH", Channel)
    xbee.atcmd("ID", PanID)
    xbee.atcmd("NI", "Sensor Node")
    xbee.atcmd("CE", 0)              # Router
    xbee.atcmd("AP", 4)              # Micropython mode
    xbee.atcmd("WR")                 # Save all settings to flash

    #Wait coordinator updates routing table
    while True:
        if xbee.atcmd("DL") != b'\x00\x00\xFF\xFF':
            print("\nROUTING TABLE UPDATED")
            break

    bledPin.off()

"""
DESCRIPTION: Boots device type based on button press
PASS : N/A
RETURN : N/A 
"""
def boot():
    #Constant ID
    choice = -1
    COORD = 0
    ROUTER = 1

    sleepTime = -1

    #Indicate ready to pair
    gledPin.on()
    while True:
        if buttonPin.value() == 0:        #if the bootPin is pressed
            time.sleep(1)
            gledPin.off()

            if buttonPin.value() == 0:    #if the bootPin is held down
                print("BOOT DATA SINK...")
                choice = COORD
                sleepTime = cBoot()

                print("\nINITIAL PING:")
                coordCallBack(NETWORK_AWAKE)
                coordSleep()
            else:
                print("BOOT SENSOR...")
                choice = ROUTER
                rBoot()

                print("\nINITIAL PING:")
                sensorCallBack(NETWORK_AWAKE)
                sensorSleep()

            #Wait for the button to be de-pressed
            while True:
                if buttonPin.value() == 1:
                    break

            break

    #Enable callbacks
    if choice == COORD:
        xbee.modem_status.callback(coordCallBack)
        print("\nCOORDINATOR CONFIGURED\n")

        #Modify sleep now that sleep schedules are synced
        changeSleep(sleepTime)

    else:
        print("\nROUTER CONFIGURED\n")
        xbee.modem_status.callback(sensorCallBack)


    print("MESSAGES")
    print("------------------------------------------------------\n")

########################################################################################

"""RESET FUNCTION"""
"""
DESCRIPTION: Resets XBee to a basic DigiMesh Node 
PASS : N/A
RETURN : N/A 
"""

def xbeeReset():
    xbee.atcmd("CH", 0xC)        #Channel
    xbee.atcmd("ID", 0X7FFF)     #PANID
    xbee.atcmd("NI", "-")        #Node identifier
    xbee.atcmd("CE", 0)          #Standard router
    xbee.atcmd("DD", 0x140000)
    xbee.atcmd("TO", 0xC0)       #DigiMesh protocol
    xbee.atcmd("NH", 0x20)       #Maximize network hops
    xbee.atcmd("MR", 3)          #Retries for MR

    xbee.atcmd("PS", 1)          #Micropython run at startup

    xbee.atcmd("ST", 0x7FF)      #quick cycle time (2+ secs)
    xbee.atcmd("SM", 0)          #No sleep
    xbee.atcmd("SO", 0)          #Non-Sleep coordinator
    xbee.atcmd("SP", 0x1)

    xbee.atcmd("DH", 0)         #Destination Address set to broadcast
    xbee.atcmd("DL", 0xFFFF)

    xbee.atcmd("NT", 0x20)      #3.2 sec discovery time

    xbee.atcmd("AV", 2)         #ADC Analog reference 3.3V

    xbee.atcmd("WR")            #Save all settings to flash

########################################################################################

"""MAIN PROGRAM"""

def main():
    print(
          "\n------------------------------------------------------\n"
          "~~~            MESH NETWORK UART DEMO              ~~~\n"
          "------------------------------------------------------\n")

    xbeeReset()
    boot()

    #Purely event driven via callbacks
    while True:
        time.sleep(120)

main()