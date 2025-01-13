# PWM-Fan-Controller
ESP32 to control a PWM fan from Delta Electronics.

This repository includes code and circuit schematics to implement a controller for a Delta Electronics PFC1212DE-F00 (PWM Fan).

## Parts (what i used):
+ Delta Electronics PFC1212DE-F00 ([used from aliexpress](https://it.aliexpress.com/item/1005006893754578.html), might work with differtent fans)
+ ESP32 DevKit ([ESP32 DevKit V1 with 38 pins](https://it.aliexpress.com/item/32959541446.html))
+ Potentiometer (3296 Trimmer Potentiometer 1.57kΩ resistance (not important))
+ 2.7 KΩ Resistor (series of two 333 Ω and a 2 KΩ resistors)
+ SSD1306 I2C OLED Display ([128x64 display](https://it.aliexpress.com/item/1005006365875586.html), using a different size one needs modifications to code)
+ PSU 12V  ≥5A ([USB-C 100W PD charger](https://it.aliexpress.com/item/1005005324613468.html) + [PD trigger board](https://it.aliexpress.com/item/1005007403613707.htm))
+ optional 0.1µF capacitor, used for better precision for the potentiometer (104 ceramic capacitor)

The code will probably work with different ESP32 boards, you can use different (more practical) potentiometer, the display is not necessary. As I wrote you will need a 12V 5A power source, I used a USB charger with P.D. and a P.D. trigger board that supports 5A and set it to 12V. You will also need another power source to power the ESP32 as it uses 5V and not 12V; you could also try to use a converter to use the same power source.

### Parts Documentation:
+ [Delta Electronics PDF1212DE-FOO](https://www.delta-fan.com/Download/Spec/PFC1212DE-F00.pdf). _Note that in my case the blue and yellow wire were inverted (yellow=F00/FG and blue=PWM)_
+ [ESP32 DevKit V1](https://lastminuteengineers.com/esp32-pinout-reference/)
+ [3296 Trimmer Potentiometer](https://components101.com/resistors/3296-trimmer-potentiometer-pinout-datasheet)
+ [SSD1306 I2C OLED Display](https://www.instructables.com/Arduino-and-the-SSD1306-OLED-I2C-128x64-Display/) _Not official but a good guide_

## Schematics:
I am not an electronic engineer, so I was not able to use any advanced electronics schematic software. I used circuit-diagram.org because it was easy for me to use.

[This](https://www.circuit-diagram.org/circuits/508e6c3034c44ba6a4b0c1beed5d6d67) is the link to the schematics, and the following is a colored version:

![schematic]


