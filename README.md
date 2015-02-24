#Smart Person Counter

##Basics

* Count the number of people passing in front of the device
* Periodically send the counter using Sigfox
* Do something with the data

##Hardware

Target was to do this using [http://snootlab.com](Snootlab)'s [Akeru Starter Kit](http://snoot.it/stkakeru)

* [Akeru](http://akeru.cc) board (Arduino Uno + TD1208 Sigfox modem)
* HC-SR04 ultrasonic sensor
* LED
* Wires :)
* Breadboard || Soldering kit


##Software

* Use the ultrasonic sensor to detect when someone/something is in front
* Count 1 person when someone *leaves* the area in front of the sensor
* Send a message through Sigfox every 15 mins
