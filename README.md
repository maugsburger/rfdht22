rfdht22
=======

RF temp/hum sensor, grown from a DHT22 and NRF24L01p based sensor to something bigger.

Originally, I just wanted to monitor humidity and temperature in all rooms of my flat, submit them wireless to a master where you can read them out and get some other fancy features (eg close all windows, now it's warmer on the outside than inside).

sensor
------

With every step of hardware design, it grew a little bit bigger, so there are two different version right now:

- either a small headless version with dht22 or one-wire and i2c, powered by attiny2313a, as cheap as possible (single sided with some bridges)

- or a big version with lcd, twi/i2c, one-wire, quad direct analogue and quad opamp driven analogue in

One design goal was to make the sensors as power saving as possible, so they will hopefully run multiple years on a pair of AA cells. No wires, no problem with WAF.

master
------

Power doesn't matter here, big ass display (currently only 4x20 character LCD), can show values of all sensors, and a lot of planned features still in my head.

possible improvements
---------------------

 - lan interface for master
 - hack some radiator controller to enable remote control
