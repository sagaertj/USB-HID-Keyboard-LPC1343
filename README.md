# README #

This code transforms an Olimex LPC-P1343 into an USB HID Keyboard input device.
All that is needed is some switcches connected between the io pins and GND.

### Requirements ###

* To compile and build you need the KEIL ARM (evaluation) tools.
* An [Olimex LPC-P1343](https://www.olimex.com/Products/ARM/NXP/LPC-P1343)
* Some Switches (footswitch or other pushbutton switch ).
* Some Soldering skills...
* Optional : Keil ULINK2 JTAG Prgrammer.

### How do I get set up? ###

* Connect the switches between GND and the io pins:
* P1.6 = Key 1
* P1.7 = Key 2
* P1.8 = Key 3
* P1.9 = Key 4
* P1.10= Key 5

* Compile and build the project.
* You can change the hardcoded keys in the GetInReport() function.
See the USB [HID Spec] (http://www.usb.org/developers/hidpage/Hut1_12v2.pdf) page 53 for details. 

### Deployment instructions
#### Using ULINK2
* Make sure Jumper BLD_E is removed.
* Connect to the host PC.
* Download the code in the CPU 
* Remove the JTAG and reset the board.

#### Without ULINK2
* Place the jumper BLD_E
* Connect to the host PC.
* The CPU will present itself as an USB disk in explorer.
* Copy the binairy to this new drive.
* Remove the power.
* Remove jumper BLD_E.
* Reconnect the power.(resetting also works without removing the usb cable first)


