# IC-9350
Codan 9350/3040 to Icom IC-7000 and IC-706 interface circuit and software.

DO NOT connect a IC-706 or an IC-7000 directly to a 5V microcontroller! The Icom radios use open-collector logic, with the IO lines pulled high to 13.8V. This will fry your microcontroller!

Included in this repository (soon) is an interface board which will handle the level conversions and drive the inputs appropriately.

Some Notes:
  * I used Arduino 1.6.4 to compile this code initially.
  * I'm using board definitions from here: http://highlowtech.org/?p=1695
  * I modified the boards.txt file (~/.arduino15/packages/attiny/hardware/avr/1.0.1/boards.txt)
    to add a 5MHz option. VK5ZM's board actually runs at 4.9125MHz, but it's close enough.
    The added section was:

    attiny.menu.clock.external5=5 MHz (external)
    attiny.menu.clock.external5.bootloader.low_fuses=0xfe
    attiny.menu.clock.external5.bootloader.high_fuses=0xdf
    attiny.menu.clock.external5.bootloader.extended_fuses=0xff
    attiny.menu.clock.external5.build.f_cpu=5000000L
