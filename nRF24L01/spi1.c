/*
 * spi1.c
 *
 *  Created on: Mar 26, 2021
 *      Author: afrid
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "gpio.h"
#include "spi1.h"

#define SSI1CLK     PORTD,0
#define SSI1FSS     PORTD,1
#define SSI1RX      PORTD,2
#define SSI1TX      PORTD,3

void initSpi1(uint32_t pinMask)
{
    // Enable the SPI clock
    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R1;
    _delay_cycles(3);
    enablePort(PORTD);

    // Configure SSI1 pins for SPI configuration
    setPinAuxFunction(SSI1CLK, GPIO_PCTL_PD0_SSI1CLK);
    selectPinPushPullOutput(SSI1CLK);
    setPinAuxFunction(SSI1TX, GPIO_PCTL_PD3_SSI1TX);
    selectPinPushPullOutput(SSI1TX);
    enablePinPullup(SSI1CLK);

    // Let the peripheral to control the chip select
    if(pinMask & USE_SSI1_FSS)
    {
        setPinAuxFunction(SSI1FSS, GPIO_PCTL_PD1_SSI1FSS);
        selectPinPushPullOutput(SSI1FSS);
    }

    if(pinMask & USE_SSI1_RX)
    {
        setPinAuxFunction(SSI1RX, GPIO_PCTL_PD2_SSI1RX);
        selectPinPushPullOutput(SSI1RX);
    }

    // Configure the SSI0 as a SPI master, mode 3, 8bit operation
    SSI1_CR1_R &= ~SSI_CR1_SSE;                        // turn off SSI to allow re-configuration
    SSI1_CR1_R = 0;                                    // select master mode
    SSI1_CC_R = 0;                                     // select system clock as the clock source
    SSI1_CR0_R = SSI_CR0_FRF_MOTO | SSI_CR0_DSS_8;     // set SCR=0, 8-bit
}

// Set baud rate as function of instruction cycle frequency
void setSpi1BaudRate(uint32_t baudRate, uint32_t fcyc)
{
    uint32_t divisorTimes2 = (fcyc * 2) / baudRate;    // calculate divisor (r) times 2
    SSI1_CR1_R &= ~SSI_CR1_SSE;                        // turn off SSI to allow re-configuration
    SSI1_CPSR_R = (divisorTimes2 + 1) >> 1;            // round divisor to nearest integer
    SSI1_CR1_R |= SSI_CR1_SSE;                         // turn on SSI
}

// Set mode
void setSpi1Mode(uint8_t polarity, uint8_t phase)
{
    SSI1_CR1_R &= ~SSI_CR1_SSE;                        // turn off SSI to allow re-configuration
    SSI1_CR0_R &= ~(SSI_CR0_SPH | SSI_CR0_SPO);        // set SPO and SPH as appropriate
    if (polarity) SSI1_CR0_R |= SSI_CR0_SPO;
    if (phase) SSI1_CR0_R |= SSI_CR0_SPH;
    SSI1_CR1_R |= SSI_CR1_SSE;                         // turn on SSI
}

// Blocking function that writes data and waits until the tx buffer is empty
void writeSpi1Data(uint32_t data)
{
    SSI1_DR_R = data;
    while (SSI1_SR_R & SSI_SR_BSY);
}

// Reads data from the rx buffer after a write
uint32_t readSpi1Data()
{
    return SSI1_DR_R;
}

