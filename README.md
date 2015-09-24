![alt text](https://raw.githubusercontent.com/sammy0025/AQILITY/master/Assets/AQILITYLogo.png)
=========================
Made by StatiX Industries

##Name:
AQILITY (Air Quality Acquisition Ability)


##Synopsis:
![alt text](https://raw.githubusercontent.com/sammy0025/AQILITY/master/Assets/EAGLEScreenshot.png  "AQILITY")

This is the open source version of the AQILITY, the free, crowd-sourced software and hardware platform for air quality detection and the upload of data to the cloud.

Sample processed (live) data which is processed by the MATLAB program included in this repo can be accessed [here](https://thingspeak.com/channels/56281).
Raw data (which may have errors) can be accessed [here](https://thingspeak.com/channels/31799)

The AVR subsystem of this system uses a forked version of the [DustDuino](http://publiclab.org/wiki/dustduino) code base (licensed under the CC BY-SA 3.0 license) for an Arduino-compatible particulate sensing system.

Features:
1. Has an LCD to display the air quality parameters dynamically on the sensor board itself
2. Automatically uploads air quality data to the cloud (ThingSpeak.com)
3. Relies on a Arduino-Raspberry Pi hybrid to achieve both long term data storage and better data transfer options such as through a cellular/3G USB dongle
4. Detects some of the most common airborne pollutants and chemicals (PM10, PM2.5, H2S, CO, VOCs) for the computation of the overall pollutant indices
5. The system has an initial waiting time, to wait for the AVR and the sensors to initialize and self-calibrate before taking readings.


##Description:
This system collects real time data from the air with several onboard sensors and relays the data from the microcontroller to the Raspberry Pi which caches all the data collected. This data is periodically uploaded onto the ThingSpeak platform, which is then analysed by the MATLAB code and the new data inserted into another ThingSpeak channel to display the actual values. The calculation from ug/m<sup>3</sup> to official AQI is a piecewise linear function provided by the EPA (Research paper [here](http://www3.epa.gov/ttn/oarpg/t1/memoranda/rg701.pdf)). The system provides an LCD screen to display live unprocessed values for easy monitoring.

A full flowchart of the system can be seen as below:

![alt text](https://raw.githubusercontent.com/sammy0025/AQILITY/master/Assets/FlowDiagram.png "Flowchart")

1. Raspberry Pi sends a single unique character (a single dot, ".", ASCII code 46) to the microcontroller
2. Microcontroller has a cached copy of all the recent data taken over a certain timeframe and sends the cached data to the Pi. This data is already formatted in the CSV format.
3. Pi saves the data from the microcontroller into a CSV database.
4. The CSV database is read by another program and the data fed into the LCD driver on the GPIO to display air quality parameters on the LCD
5. Simultaneously, this data is sent by a HTTP POST request to the first ThingSpeak channel, which contains unprocessed data.
6. Data crunching takes place on the ThingSpeak platform with the MATLAB language, which calculates the correct AQI value using a piecewise linear function defined by the EPA.
7. The processed data is sent to the second ThingSpeak channel which houses all the processed data.


##Availability:
This system is only tested on Raspberry Pis and the ATmega328P chip. There will be no guarentees that the same code (built on the Arduino platform) will run properly on other Arduino platforms or even on other AVRs such as the XMega series. The ribbon cable connector is customized for Raspberry Pi + series ONLY, but you can manually pinout the connector for the old 28-pin GPIO.

##Author(s):
* Chief Designer; Programmer: Pan Ziyue
* R&D/Scientific Inquiry: Chow Zi Jie & Rachit Agrwal
* The awesome open source folks at Adafruit and the [DustDuino Development Team](http://publiclab.org/wiki/dustduino)


##Caveats:
* Requires for the Raspberry Pi's UART port on the GPIO to be freed. More info [here](http://www.hobbytronics.co.uk/raspberry-pi-serial-port)
* The code on the AVR seems to have a discrepancy of a factor of 10 for the PM2.5 concentration, which is then manually rectified by the MATLAB code. There may be a serious underlying problem with the original DustDuino code for the calculation of concentration, or lack of calibration of the sensor (which is supposed to be factory calibrated)
* The use of a Raspberry Pi enforces the use of a 5V/3.3V logic level shifter to avoid damaging the Pi.
* The gas sensors have NO calibration, which means that there is currently no logic implemented for the equation of output voltage of gas sensors to corresponding concentrations of gases.
* The Python code on the Raspberry Pi does not automatically archive or delete old data, and requires manual work to clear old data. This is a possible area of improvement and we welcome you to submit a pull request to implement such a feature.
* Dependencies must be manually installed and the GPIO port manually configured, which reduces code reusability and portability. Suggest making a single shell script to accomplish all of that.
* Python scripts must be specified in `/etc/rc.local` for it to automatically start when the Raspberry Pi is booted up.


##Dependencies:
* [Adafruit_Python_CharLCD](https://github.com/adafruit/Adafruit_Python_CharLCD)
* [DHT11 Library](http://playground.arduino.cc/Main/DHT11Lib)
* PySerial (install with `sudo pip3 install pyserial`)
* [httplib2](https://github.com/jcgregorio/httplib2)

##License:
<a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/3.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/">Creative Commons Attribution-ShareAlike 3.0 Unported License</a>.

Copyright (C) StatiX Industries 2014-2016
