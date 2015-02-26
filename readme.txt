Arduino Thermistor Library

A lightweight library for using thermistors at high accuracy.

Features:

Uses a Look Up Table (LUT) with interpolation for maximum accuracy; 
 for when a linear approximation just isn't good enough.
Stores the LUT in flash to conserve valuable RAM on the Arduino.
Implements a binary search to be blazing fast, even with long tables.
Easily extensible

Usage: 

If you happen to use the same thermistors and resistors as me, great! Paste the 
library into a folder named "thermistor", import the library into Arduino IDE
and go for it.
       
More likely, you'll need to make a LUT for your Thermistor(s). Fear not, the 
included thermistor.xls provides an excellent tool for picking your resistor and
generating a lookup table.

thermistor.xls:

1. Get the table of resistance values vs. degrees C from your thermistor vendor.
   (If they don't offer a table, don't use them)
2. Enter the values into that T and R columns of the spreadsheet.
3. Enter the range of temperatures of interest into the Low and High target temp
    - This helps make the graphs and error calculation useful
3. Play with the "Chosen R" value. This is the series resistor you'll use to
    convert your variable resistance to a variable voltage.
    - Try to get a lot of voltage change over your temperature range in the 
      top graph
    - Alternatively, try to get Max Error low.
*** The top graph and "Max Error" show the results of a linear approximation
    If you get your error low enough for your needs, consider just using the
    Linear Fit; it will be faster than the LUT.
4. Grab the values from "Voltage (16 bit)" and create an array out of them
5. Follow the remaining instructions in thermistor.cpp to add your values
6. Hook up your thermistor as follows:

           5V
          _____
            |
            -
           | | Fixed Resistor (Chosen R)
           | |
            -
            |
            |--------- Arduino Analog Pin
            |
            - |
           | |/ Thermistor
           |/|
           /-
            |
          -----
           ---  GND 
            -

7. Sample the resistor in your sketch, then feed the value to the appropriate
   getTemp function.
8. For even better resolution, sample the value 2^n times (n <= 6), accumulating 
   the value, then call thermistor_setInputShift(6-n).
    - This allows you to average together several samples, giving you a more
      accurate reading
    - You must inform the library that you are doing this, so it knows how big
      the numbers coming in will be.
    - Note that averaging these values doesn't work; Arduino uses integers,
      which can't handle decimals! (233+234)/2 = 233!