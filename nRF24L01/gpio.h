// GPIO Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// GPIO APB ports A-F

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include <stdbool.h>

// Enum values set to bitband address of GPIO_PORTx_DATA_R register
typedef enum _PORT
{
    PORTA = 0x42000000 + (0x400043FC-0x40000000)*32,
    PORTB = 0x42000000 + (0x400053FC-0x40000000)*32,
    PORTC = 0x42000000 + (0x400063FC-0x40000000)*32,
    PORTD = 0x42000000 + (0x400073FC-0x40000000)*32,
    PORTE = 0x42000000 + (0x400243FC-0x40000000)*32,
    PORTF = 0x42000000 + (0x400253FC-0x40000000)*32
} PORT;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void enablePort(PORT port);
void disablePort(PORT port);

void selectPinPushPullOutput(PORT port, uint8_t pin);
void selectPinOpenDrainOutput(PORT port, uint8_t pin);
void selectPinDigitalInput(PORT port, uint8_t pin);
void selectPinAnalogInput(PORT port, uint8_t pin);
void setPinCommitControl(PORT port, uint8_t pin);

void enablePinPullup(PORT port, uint8_t pin);
void disablePinPullup(PORT port, uint8_t pin);
void enablePinPulldown(PORT port, uint8_t pin);
void disablePinPulldown(PORT port, uint8_t pin);

void setPinAuxFunction(PORT port, uint8_t pin, uint32_t fn);

void selectPinInterruptRisingEdge(PORT port, uint8_t pin);
void selectPinInterruptFallingEdge(PORT port, uint8_t pin);
void selectPinInterruptBothEdges(PORT port, uint8_t pin);
void selectPinInterruptHighLevel(PORT port, uint8_t pin);
void selectPinInterruptLowLevel(PORT port, uint8_t pin);
void enablePinInterrupt(PORT port, uint8_t pin);
void disablePinInterrupt(PORT port, uint8_t pin);

void setPinValue(PORT port, uint8_t pin, bool value);
bool getPinValue(PORT port, uint8_t pin);
void setPortValue(PORT port, uint8_t value);
uint8_t getPortValue(PORT port);

#endif
