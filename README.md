# ircontrol_neptune_apex

Arduino program to control Air Conditioner using IR Signal. Used mainly to cool aquarium water using Air Conditioner  and Neptune Apex controller.
On power outlet the board send the programed ON IR Code and on shutdown the outled, the board send the OFF IR code.

Brief hardware description:
---

Used a Arduino nano with the follow connections:

PIN D2 - Programming STATUS LED
PIN D3 - IR LED Output 
PIN D4 - Program ON Code Switch ( 0 - Pressed )
PIN D5 - Program OFF Code Switch ( 0 - Pressed )
PIN D6 - AC Line Status ( 1 - Outlet ON; 0 - Outlet OFF)
PIN D11 - IR Receiver ( TSOP38238 or compatible )
PIN D13 - Fail Reading IR Code (On board LED)


The hardware works in a simplified way like this:
We have a 5V power supply with large capacitors (about 20.000 uF) at output to provide some life time after AC disconection.

Using an optocoupler (PC817) we monitor the AC outlet quickly through pin D6. As soon as the AC has been interrupted we need send the IR Code to Air conditioner turn off. For this stage we rectify using a diode Bridge and use a resistor to limit current to internal led of PC817. The transisor side of opto-coupler, we use and NPN inverter and a capacitor (lower value 10nF) to remove the ripple of AC.


Brief hardware description:
---

The firmware basis is the IRLibrary (http://z3t0.github.io/Arduino-IRremote/). 
What we do it's just keep monitoring the PIN D6 (AC line Status) and as soon as it change, we send the correspondant command to IR. The programmed command is saved at eeprom.

When it's necessary reprogram the codes, the user just press the switch of desired command (ON or OFF) and press the remote control corresponding key.


