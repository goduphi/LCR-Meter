// GPIO Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL with LCD/Keyboard Interface
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// GPIO APB ports A-F

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "gpio.h"

#define OFS_DATA_TO_DIR    1*4*8
#define OFS_DATA_TO_IS     2*4*8
#define OFS_DATA_TO_IBE    3*4*8
#define OFS_DATA_TO_IEV    4*4*8
#define OFS_DATA_TO_IM     5*4*8
#define OFS_DATA_TO_AFSEL  9*4*8
#define OFS_DATA_TO_ODR   68*4*8
#define OFS_DATA_TO_PUR   69*4*8
#define OFS_DATA_TO_PDR   70*4*8
#define OFS_DATA_TO_DEN   72*4*8
#define OFS_DATA_TO_CR    74*4*8
#define OFS_DATA_TO_AMSEL 75*4*8

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void enablePort(PORT port)
{
    switch(port)
    {
        case PORTA:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
            SYSCTL_GPIOHBCTL_R &= ~1;
            break;
        case PORTB:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
            SYSCTL_GPIOHBCTL_R &= ~2;
            break;
        case PORTC:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;
            SYSCTL_GPIOHBCTL_R &= ~4;
            break;
        case PORTD:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
            SYSCTL_GPIOHBCTL_R &= ~8;
            break;
        case PORTE:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
            SYSCTL_GPIOHBCTL_R &= ~16;
            break;
        case PORTF:
            SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
            SYSCTL_GPIOHBCTL_R &= ~32;
    }
    _delay_cycles(3);
}

void disablePort(PORT port)
{
    switch(port)
    {
        case PORTA:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R0;
            break;
        case PORTB:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R1;
            break;
        case PORTC:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R2;
            break;
        case PORTD:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R3;
            break;
        case PORTE:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R4;
            break;
        case PORTF:
            SYSCTL_RCGCGPIO_R &= ~SYSCTL_RCGCGPIO_R5;
    }
    _delay_cycles(3);
}

void selectPinPushPullOutput(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_ODR;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DIR;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DEN;
    *p = 1;
}

void selectPinOpenDrainOutput(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_ODR;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DIR;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DEN;
    *p = 1;
}

void selectPinDigitalInput(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DIR;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DEN;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_AMSEL;
    *p = 0;
}

void selectPinAnalogInput(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_DEN;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_AMSEL;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_AFSEL;
    *p = 1;
}

void setPinCommitControl(PORT port, uint8_t pin)
{
    switch(port)
    {
        case PORTA:
            GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTB:
            GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTC:
            GPIO_PORTC_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTD:
            GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTE:
            GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;
            break;
        case PORTF:
            GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    }
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_CR;
    *p = 1;
}

void enablePinPullup(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_PUR;
    *p = 1;
}

void disablePinPullup(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_PUR;
    *p = 0;
}

void enablePinPulldown(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_PDR;
    *p = 1;
}

void disablePinPulldown(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_PDR;
    *p = 0;
}

void setPinAuxFunction(PORT port, uint8_t pin, uint32_t fn)
{
    // call with header file shifted values or 4-bit number
    if (fn <= 15)
        fn = fn << (pin*4);
    else
        fn = fn & (0x0000000F << (pin*4));
    switch(port)
    {
        case PORTA:
            GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTB:
            GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTC:
            GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTD:
            GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTE:
            GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
            break;
        case PORTF:
            GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R & ~(0x0000000F << (pin*4))) | fn;
    }
    // set AFSEL bit only if using aux function, otherwise clear bit
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_AFSEL;
    *p = (fn > 0);
}

void selectPinInterruptRisingEdge(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IBE;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IEV;
    *p = 1;
}

void selectPinInterruptFallingEdge(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IBE;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IEV;
    *p = 0;
}

void selectPinInterruptBothEdges(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 0;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IBE;
    *p = 1;
}

void selectPinInterruptHighLevel(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IEV;
    *p = 1;
}

void selectPinInterruptLowLevel(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IS;
    *p = 1;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IEV;
    *p = 0;
}

void enablePinInterrupt(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IM;
    *p = 1;
}

void disablePinInterrupt(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin + OFS_DATA_TO_IM;
    *p = 0;
}

void setPinValue(PORT port, uint8_t pin, bool value)
{
    uint32_t* p;
    p = (uint32_t*)port + pin;
    *p = value;
}

bool getPinValue(PORT port, uint8_t pin)
{
    uint32_t* p;
    p = (uint32_t*)port + pin;
    return *p;
}

void setPortValue(PORT port, uint8_t value)
{
    switch(port)
    {
        case PORTA:
            GPIO_PORTA_DATA_R = value;
            break;
        case PORTB:
            GPIO_PORTB_DATA_R = value;
            break;
        case PORTC:
            GPIO_PORTC_DATA_R = value;
            break;
        case PORTD:
            GPIO_PORTD_DATA_R = value;
            break;
        case PORTE:
            GPIO_PORTE_DATA_R = value;
            break;
        case PORTF:
            GPIO_PORTF_DATA_R = value;
    }
}

uint8_t getPortValue(PORT port)
{
    uint8_t value;
    switch(port)
    {
        case PORTA:
            value = GPIO_PORTA_DATA_R;
            break;
        case PORTB:
            value = GPIO_PORTB_DATA_R;
            break;
        case PORTC:
            value = GPIO_PORTC_DATA_R;
            break;
        case PORTD:
            value = GPIO_PORTD_DATA_R;
            break;
        case PORTE:
            value = GPIO_PORTE_DATA_R;
            break;
        case PORTF:
            value = GPIO_PORTF_DATA_R;
    }
    return value;
}
