#!/usr/bin/python
# serialReader.py Python Code (part of the AQILITY project)
# This script *SHOULD* be started on boot
# Script requires the following dependencies:
# 1. serial
import serial
import csv
import time
import signal
import sys

shutdown = False

print("Starting AQILITY helper daemon")

#TODO: Change the serial port to something else, unless you are using the default Raspberry Pi UART port over the GPIO
ser = serial.Serial("/dev/ttyAMA0", 9600)
print("TTY session opened")

print("Waiting for Arduino...")
# Sleep for 2 minutes for data to stabilize before sending anything after TTY initiation
time.sleep(120)
print("Arduino is responding, initiate handshake protocol")

# Make SIGTERM handler to gracefully terminate the connection
def signal_term_handler(signal, frame):
    shutdown = True
    time.sleep(2)
    sys.exit(0)
signal.signal(signal.SIGTERM, signal_term_handler)

try:
    while shutdown == False:
        print("Requesting transmission")
        ser.write(".") # Request for data transmission
        tried = ser.readline()
        tried = tried[:-2] # Truncate last two characters (they are \n and \r)
        # Reference database format: MQ131,MQ135,MQ9/1,MQ9/2,MQ136,PM2.5,PM10,AQI,TEMP,HUM
        with open("database.csv","a") as f:
            f.write(tried)
            f.write("\n")
        print("Results written to file")
        time.sleep(60) # Wait for a minute for next serial transmission
except:
    print("Termination signal received, closing serial connection...")
