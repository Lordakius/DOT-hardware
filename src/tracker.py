import binascii
import sys
import RPi.GPIO as GPIO
import time
import Adafruit_PN532 as PN532
import requests
from uuid import getnode as get_mac
from datetime import datetime

#Function definitions
def lightLED(LEDPIN, duration):
    GPIO.output(LEDPIN, GPIO.HIGH)
    time.sleep(duration)
    GPIO.output(LEDPIN, GPIO.LOW)
    return

#Setup for the API requests
URL = "https://dositrackingapi.herokuapp.com/attendance"
headers = {"Content-Type":"application/json"}

# Setup how the PN532 is connected to the Raspbery Pi/BeagleBone Black.
# It is recommended to use a software SPI connection with 4 digital GPIO pins.

# Configuration for a Raspberry Pi:
CS   = 20
MOSI = 13
MISO = 19
SCLK = 26
GREENLEDPIN = 4
REDLEDPIN = 17
BLUELEDPIN = 27

#Setup GPIO Output
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(GREENLEDPIN, GPIO.OUT)
GPIO.setup(REDLEDPIN, GPIO.OUT)
GPIO.setup(BLUELEDPIN, GPIO.OUT)

# Create an instance of the PN532 class.
pn532 = PN532.PN532(cs=CS, sclk=SCLK, mosi=MOSI, miso=MISO)

# Call begin to initialize communication with the PN532.  Must be done before
# any other calls to the PN532!
pn532.begin()

# Get the firmware version from the chip and print(it out.)
ic, ver, rev, support = pn532.get_firmware_version()
print('Found PN532 with firmware version: {0}.{1}'.format(ver, rev))

# Configure PN532 to communicate with MiFare cards.
pn532.SAM_configuration()

# Main loop to detect cards and read a block.
print('Waiting for MiFare card...')
while True:
    # Check if a card is available to read.
    uid = pn532.read_passive_target()
    # Try again if no card is available.
    if uid is None:
        continue
    print('Found card with UID: 0x{0}'.format(binascii.hexlify(uid)))
    print(int(binascii.hexlify(uid), 16))
    GPIO.output(BLUELEDPIN, GPIO.HIGH)

    payload = {"rfid":int(binascii.hexlify(uid),16) , "deviceid":str(hex(get_mac())) , "time":datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
    print(payload)
    response = requests.post(URL, headers=headers, json=payload)
    print(response.status_code, response.reason)

    GPIO.output(BLUELEDPIN, GPIO.LOW)

    if(response.status_code == 200):
        lightLED(LEDPIN=GREENLEDPIN, duration=0.3)
    else:    
        lightLED(LEDPIN=REDLEDPIN, duration=0.3)
