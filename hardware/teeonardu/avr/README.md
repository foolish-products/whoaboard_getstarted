TeeOnArdu
=========
Running Teensy2.0 programs on Arduino Leonardo or
-------------------------------------------------
Finally got USB-MIDI to run easily on Arduino Leonardo inside Arduino-IDE
-------------------------------------------------------------------------

Hi there! Ladyada here! This is a fork of the really awesome TeeOnArdu written by Georg Werner. 
I have made a few minor adjustments to make this add-on Flora compatible and also not require a new bootloader.

It's currently in beta. Only MIDI works at all - not supported or tested beyond like "hey midiOx didnt crash!"

====

In workshops and classes i teach electronics and programming for artistic purposes (building instruments, controllers and installations). I mostly use open source technology because i think it fits the idea of looking into things to understand them and it thrives on the exchange of ideas.

Why USB_MIDI?
-------------
For example Arduino and Pure Data work quite well together through Arduinos serial interface, but many sound softwares need extra software running as a proxy to work with Arduino. Most of them understand MIDI for input and output. So in 2011 with friends i designed and built a board the [Kaputtverboten](http://kaputverboten.com/) based on [V-USB](http://www.obdev.at/products/vusb/index.html) and configurable via MIDI. It worked very well and we even gave a workshop with it at the [Pure Data Convention 2011](http://www.uni-weimar.de/medien/wiki/PDCON:Workshops/Build_your_own_USB-MIDI-Interface) but we never wanted to sell it apart from the workshops. And with the introduction of the Leonardo i thought there would be finally an Arduino that talks MIDI out of the box.
With the arrival of Arduino Leonardo with it's ATMega32u4 it could have been possible to have a USB-MIDI Device developed directly inside the Arduino IDE but the Arduino Team only implemented keyboard and mouse functionality.

Current situation
-----------------
So there was no possibility to turn the Leonardo easily into a USB-MIDI device. By easily i mean for example turn it into USB-MIDI and back in the normal Leonardo function without flashing firmware each time. To use [HIDuino](http://dimitridiakopoulos.com/hiduino) and [MOCO](http://morecatlab.akiba.coocan.jp/lab/index.php/aruino/midi-firmware-for-arduino-uno-moco/?lang=en) you have to do exactly that - so they were out of the race. Another way is to change the core files of Arduino like [arcore](https://github.com/rkistner/arcore) - but then you are tied to a specific Arduino-IDE version if you don't want to change the files with every new version of Arduino.

Solution
--------
After waiting for years to have somebody fill the gap i sat down and tackled the problem myself. I knew it should be possible since the [Teensy](http://www.pjrc.com/teensy/) environment shows that it could be done. The Teensy 2.0 has the same ATMega32u4 Processor as the the Leonardo. And there is the possibility to program Teensy boards as USB HID devices and USB MIDI devices with the Arduino IDE ([Teensyduino](http://www.pjrc.com/teensy/teensyduino.html))
The compiled .hex files from Teensyduino for Teensy 2.0 can be programmed onto the Leonardo with an ISP programmer but that is a rather complicated procedure compared to usual Arduino programming.

After trying different routes to include USB MIDI into Leonardo i decided to create a custom board config file and core from Teensy that can be easily placed inside the Arduino>Hardware folder in your home directory. As a side effect you can now use all Teensyduino code for Teensy 2.0 on the Leonardo hardware not only MIDI. Obviously this has to be called "TeeOnArdu".

There were two stumble stones:

1.    Teensy and Leonardo have a different board layout so the pin number assignment is different. I changed that in pins_arduino.h/pins_teensy.c & core_pins.h - this is not well tested but seems to work (alpha)
2.    there is a bug in the bootloader of the Leonardo (called Caterina) which prevents the execution of the Teensy code - it hangs right after the bootloader timed out. I found the solution for that in the [AVRFreaks forum](http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=123862) and changed the bootloader accordingly. It's included in TeeOnArdu>bootloaders>caterina. You have to burn it to your Leonardo to use TeeOnArdu. (you can use another Arduino to do it - look at [ArduinoISP](http://arduino.cc/en/Tutorial/ArduinoISP))  (I fixed this, no new bootloader required! --ladyada)

How to use it
-------------

*	Unzip the TeeOnArdu.zip and copy its contents either to the Arduino>hardware folder or to a folder called "hardware" inside your sktech folder.
*	When you start Arduino the next time there will be a new entry under Tools>Board>"TeeOnArdu". (just like the [ATTiny cores](http://code.google.com/p/arduino-tiny/source/browse/README)) Select it.
*	Select Tools>USB Type>MIDI.
*	Copy the USB-MIDI example code from Teensy http://www.pjrc.com/teensy/td_midi.html to the Arduino IDE and upload it.
*	Your Leonardo is now a fully standard complient USB-MIDI device. No need to install drivers in any OS. But because of that there is also no serial port USB-CDC anymore. Don't worry it's still inside the bootloader.
*	To upload new sketches you just have to press the reset button each time you hit the upload button. Like in the old days of the first Arduino generations :)
*	If you decide to use your Leonardo as Leonardo again just select it under Tools>Board>Leonardo and upload an Arduino sketch like Blink File>Examples>Basics>Digital>Blink. The first time you have to press the reset button but after that your Leonardo behaves like out of the box.

Authors
-------

*	Georg Werner  -  port Teenys code to Arduino Leonardo IDE [georgwerner.de](http://georgwerner.de)

based on work of
----------------
*	Paul J Stoffregen  -  Teensy code [www.pjrc.com](http://www.pjrc.com)
*	Dean Camera  -  LUFA (base of the Leonardo bootloader) [www.fourwalledcubicle.com](http://www.fourwalledcubicle.com)

   
