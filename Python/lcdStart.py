#!/usr/bin/python
# lcdStart.py Python Code (part of the AQILITY project)
# This script *SHOULD* be started on boot
# This script also contains code for the automatic upload of raw data to ThingSpeak
# Script requires the following dependencies:
# 1. Adafruit_CharLCD
# 2. httplib2
import math
import time
import Adafruit_CharLCD as LCD
import csv
import signal
import sys
from urllib import urlencode
from httplib2 import Http

# Raspberry Pi pin configuration:
lcd_rs        = 27  # Note this might need to be changed to 21 for older revision Pi's.
lcd_en        = 22
lcd_d4        = 25
lcd_d5        = 24
lcd_d6        = 23
lcd_d7        = 18
lcd_backlight = 4

# Define LCD column and row size for 16x2 LCD.
lcd_columns = 16
lcd_rows    = 2

# Alternatively specify a 20x4 LCD.
# lcd_columns = 20
# lcd_rows    = 4

# Initialize the LCD using the pins above.
lcd = LCD.Adafruit_CharLCD(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7,
                            lcd_columns, lcd_rows, lcd_backlight)

lcd.clear()

# Print a dynamic message that spans one minute
for x in range(0,80):
    lcd.message('AQILITY v0.1b\nStarting up...')
    time.sleep(0.5)
    lcd.clear()
    lcd.message('AQILITY v0.1b\nStarting up.')
    time.sleep(0.5)
    lcd.clear()
    lcd.message('AQILITY v0.1b\nStarting up..')
    time.sleep(0.5)
    lcd.clear()

# Make SIGTERM handler to make the LCD look nicer when RPi is shutting down
def signal_term_handler(signal, frame):
    lcd.clear()
    lcd.message('AQILITY v0.1b\nShutting down')
    sys.exit(0)
signal.signal(signal.SIGTERM, signal_term_handler)

# Read from database and populate screen with data
lcd.clear()
# Reference database format: MQ131,MQ135,MQ9/1,MQ9/2,MQ136,PM2.5,PM10,AQI,TEMP,HUM
# Local variables in this dictionary:
localVariables = {'mq131Voltage':"",'mq135Voltage':"",'mq9Voltage':"",'mq92Voltage':"",'mq136Voltage':"",'pm25':"",'pm10':"",'AQI':"",'temp':"",'hum':""}
# Make function that counts lines in the CSV
def file_len(fname):
    with open(fname) as f:
        for i, l in enumerate(f):
            pass
    return i + 1

# Begin loop
while True:
    # Update all variables
    data = [row for row in csv.reader(open('database.csv', 'rb'))]
    fileLength = file_len('database.csv')
    localVariables['mq131Voltage']=data[fileLength-1][0]
    localVariables['mq135Voltage']=data[fileLength-1][1]
    localVariables['mq9Voltage']=data[fileLength-1][2]
    localVariables['mq92Voltage']=data[fileLength-1][3]
    localVariables['mq136Voltage']=data[fileLength-1][4]
    localVariables['pm25']=data[fileLength-1][5]
    localVariables['pm10']=data[fileLength-1][6]
    localVariables['AQI']=data[fileLength-1][7]
    localVariables['temp']=data[fileLength-1][8]
    localVariables['hum']=data[fileLength-1][9]
    try:
        h = Http()
        #TODO: insert ThingSpeak API key here, previous key redacted for security reasons
        data = dict(key="",
        field1=localVariables['pm25'],
        field2=localVariables['pm10'],
        field3=localVariables['temp'],
        field4=localVariables['hum'],
        field5=localVariables['mq131Voltage'],
        field6=localVariables['mq135Voltage'],
        field7=localVariables['mq136Voltage'],
        field8=localVariables['mq92Voltage'])
        resp, content = h.request("http://api.thingspeak.com/update","POST",urlencode(data))
    except:
        lcd.message("NO INTERNET CONN\nNO DATA UPLOADED")
        time.sleep(5)
        lcd.clear()
    for r in range(0,2):
		# Show PM2.5+PM10+AQI
		for i in range(0,2):
			lcd.message("PM2.5:"+localVariables['pm25']+"\nPM10:"+localVariables['pm10'])
			time.sleep(2)
			lcd.clear()
			lcd.message("AQI Rating:\n"+localVariables['AQI'])
			time.sleep(2)
			lcd.clear()
		# Show MQ Gas Sensors:
		for i in range(0,2):
			lcd.message("MQ Gas Sensors:\n"+"MQ131(O3):"+localVariables['mq131Voltage']+"V")
			time.sleep(2)
			lcd.clear()
			lcd.message("MQ135(VOC):"+localVariables['mq135Voltage']+"V\n"+"MQ136(SO2):"+localVariables['mq136Voltage']+"V")
			time.sleep(2)
			lcd.clear()
			lcd.message("MQ9(LPG):"+localVariables['mq9Voltage']+"V\n"+"MQ9(CO):"+localVariables['mq92Voltage']+"V")
			time.sleep(2)
			lcd.clear()
		# Show DHT11 data
		for i in range(0,2):
			lcd.message("Temperature:\n"+localVariables['temp']+chr(223)+"C")
			time.sleep(2)
			lcd.clear()
			lcd.message("Humidity:\n"+localVariables['hum']+chr(37))
			time.sleep(2)
			lcd.clear()
