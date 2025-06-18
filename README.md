<a href="https://www.microchip.com" rel="nofollow"><img src="images/microchip.png" alt="MCHP" width="300"/></a>

# ATtiny416 Precise ULP Timing on P4

This example is a demonstration of precise, ultra-low-power timing using periodic enabling of the 32.768 kHz external crystal oscillator on the AVR P4 FEB (Field Engagement Board).

The example is explained in more details in the application note [AN2633](https://www.microchip.com//wwwAppNotes/AppNotes.aspx?appnote=en604346).

## Related Documentation

- [AN2633 - Precise, Ultra-Low-Power Timing using Periodic Enablingof the 32.768 kHz External Crystal Oscillator forRecalibration of the ULP Internal Oscillator](https://www.microchip.com//wwwAppNotes/AppNotes.aspx?appnote=en604346)
- [ATtiny416 Product Page](https://www.microchip.com/wwwproducts/en/ATtiny416)

## Software Used

- [MPLAB X IDE v6.25 or later](https://www.microchip.com/mplab/mplab-x-ide)
- [XC8 (v3.00)](https://www.microchip.com/mplab/compilers) alternatively [AVR/GNU C Compiler 5.4.0](https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers) can be used
- ATtiny_DFP 3.3.272 or later

## Hardware Used

-  [AVR P4](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/ATAVRFEB-P4)

## Operation

1. Download the zip file or clone the example to get the source code.
2. Open `attiny416-precise-ulp-timing-p4-mplab.X` in MPLAB.
3. Connect the AVR P4 with your computer.
4. Make sure the kit is selected as the tool to be programmed under project settings.
5. Press the make and program button to program the device.

## Conclusion

We have here shown the possibility to use the ATtiny416 for ultra-low-power timing on a AVR P4.