#Smart Person Counter

##Basics

* Count the number of people passing in front of the device
* Periodically send the counter using Sigfox
* Do something with the data

##Hardware

* [Arduino Uno](http://arduino.cc ) board
* [Akene](http://snoot.it/akene) Arduino Shield, from [Snootlab](http://snootlab.com)
* HC-SR04 ultrasonic sensor
* LED
* Wires :)
* Breadboard || Soldering kit

Beware ! The Akene lib is using pins 4 & 5 to send data to the SIGFOX modem. Stay away from them ;)


##Software

* Use the ultrasonic sensor to detect when someone/something is in front
* Count 1 person when someone *leaves* the area in front of the sensor
* Send a message through Sigfox every 10 mins
