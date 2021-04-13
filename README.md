# LCR Meter

The aim of this project was to build a budget LCR meter which can measure Resistance, Capacitance, and Inductance with a reasonable amount of
accuracy.

## Technologies used
- Tiva C Series (TM4C123GH6PM)
  - Peripherals: GPIO, UART, Timer, ADC, Analog Comparator
  - IDE: Code Composer Studio
  - Programming Language: C
  
## Tools used for debugging
- Hardware
  - Digital Multimeter
  - Oscilloscope
- Software
  - Code Composer Studio Debugger


## Parts List
Part | Quantity
---- | --------
2N3904 NPN transistor | 5
2N3906 PNP transistor | 2
33ohm, 1/2W resistor | 1
3.3kohm, 1/4W resistor | 7
10kohm 1/4W resistor | 7
100kohm, 1/4W resistor | 1
1N5819 Schottky diode (flyback diodes) | 4
1uF capacitor (integrator) | 1
47uF capacitor (power supply) | 1
2x10 double-row header, unshrouded | 2
HD44780 LCD display | 1
Pushbuttons | ~6
Wire (22-24 AWG solid wire, 3+ colors) | 1
PC board (approx 4.5x6”) | 1

# Design goal of the circuit
I was always fascinated by PCB fabrication and due to not having direct access to that I decided to make my wiring resemble PCB traces
to the best of abilities. This is not the best design as the solder joints might break if enough force is applied on them, but they are
strong enough to withstand quite a bit of impact.
