# IC-9350
Codan 9350/3040 to Icom IC-7000 and IC-706 interface circuit and software.

DO NOT connect a IC-706 or an IC-7000 directly to a 5V microcontroller! The Icom radios use open-collector logic, with the IO lines pulled high to 13.8V. This will fry your microcontroller!

Included in this repository (soon) is an interface board which will handle the level conversions and drive the inputs appropriately.

Some Notes:
  * I used Arduino 1.6.4 to compile this code initially.
  * I'm using board definitions from here: http://highlowtech.org/?p=1695
  * Refer to the comment block at the top of Icom9350.ino for more information, including wiring info.


TODO
----
* Handle scan mode (tuner preamp on) appropriately. The only indicators we have from the Icom are 'tune' and 'tuner reset'. Since it takes the 9350 about 10 seconds to finish 'homing' the stepper upon entering scan mode, it's not such a good idea to go into scan mode on every tuner reset.
* Upload PCB & schematic designs for the interface PCB.
