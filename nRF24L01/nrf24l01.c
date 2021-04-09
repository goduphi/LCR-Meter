/*
 * nrf24l01.c
 *
 *  Created on: Apr 6, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "spi1.h"
#include "nrf24l01.h"

#define CSN                 PORTD,1
#define CHIP_ENABLE         PORTD,6

// Registers
#define CONFIG            0x00
#define EN_AA               0x01
#define EN_RXADDR           0x02
#define SETUP_AW            0x03    // Sets the width of the address
#define SETUP_RETR          0x04
#define RF_CH               0x05
#define RF_SETUP            0x06
#define STATUS              0x07
#define OBSERVE_TX          0x08
#define CD                  0x09
#define RX_ADDR_P0          0x0A
#define RX_ADDR_P1          0x0B
#define RX_ADDR_P2          0x0C
#define RX_ADDR_P3          0x0D
#define RX_ADDR_P4          0x0E
#define RX_ADDR_P5          0x0F
#define TX_ADDR             0x10
#define RX_PW_P0            0x11
#define RX_PW_P1            0x12
#define RX_PW_P2            0x13
#define RX_PW_P3            0x14
#define RX_PW_P4            0x15
#define RX_PW_P5            0x16
#define FIFO_STATUS         0x17
#define DYNPD               0x1C
#define FEATURE             0x1D

#define MAX_ADDRESS_LENGTH  4
#define MAX_DATA_BYTES      32

typedef enum _mode
{
    POWER_DOWN, RX, TX
} mode;

uint8_t rxBuffer[MAX_DATA_BYTES];

void chipEnable()
{
    selectPinPushPullOutput(CHIP_ENABLE);
}

void rfCsOff()
{
    setPinValue(CSN, 0);
    _delay_cycles(4);                    // allow line to settle
}

void rfCsOn()
{
    setPinValue(CSN, 1);
}

void rfWriteRegister(uint8_t reg, uint8_t data)
{
    rfCsOff();
    // Select register
    writeSpi1Data(W_REGISTER | reg);
    // Read dummy data to clear buffer
    readSpi1Data();
    // Write dummy data
    writeSpi1Data(data);
    // Read dummy data to clear buffer
    readSpi1Data();
    rfCsOn();
}

uint8_t rfReadRegister(uint8_t reg)
{
    uint8_t data = 0;
    rfCsOff();
    // Select register
    writeSpi1Data(R_REGISTER | reg);
    // Read dummy data to clear buffer
    readSpi1Data();
    // Write dummy data
    writeSpi1Data(NOP);
    // Read register data
    data = readSpi1Data();
    rfCsOn();
    return data;
}

/*
 * Reads n bytes into a buffer
 */
void rfReadIntoBuffer(uint8_t reg, uint8_t buffer[], uint8_t nBytes)
{
    rfCsOff();
    // Select register
    writeSpi1Data(R_REGISTER | reg);
    // Read dummy data to clear buffer
    readSpi1Data();
    uint8_t i = 0;
    for(i = 0; i < nBytes; i++)
    {
        // Write dummy data
        writeSpi1Data(NOP);
        // Read register data
        buffer[i] = readSpi1Data();
    }
    rfCsOn();
}

void rfSetMode(mode m)
{
}

/*
 * This function sets the address of the rf module.
 * The max address length allowed is 4 bytes.
 * Refer to page 35 of the datasheet to know more about data pipes
 */
void rfSetAddress(uint8_t pipe, uint32_t address)
{
    // The address width is always 4 bytes - 0x10
    rfWriteRegister(SETUP_AW, 0x10);
    rfCsOff();
    // Select the address (pipe) register
    writeSpi1Data(W_REGISTER | pipe);
    // Read dummy data to clear buffer
    readSpi1Data();
    // Write the address to the register
    uint8_t i = 0;
    for(i = 0; i < MAX_ADDRESS_LENGTH; i++)
    {
        // Write lower LSB first
        writeSpi1Data(((address >> (i << 3)) & 0xFF));
        readSpi1Data();
    }
    rfCsOn();
}

void initNrf24l01(uint32_t address)
{
    initSpi1(USE_SSI1_RX);
    selectPinPushPullOutput(CSN);
    // Run at 8Mbps
    setSpi1BaudRate(5e6, 40e6);
    setSpi1Mode(0, 0);

    // Only use one data pipe to transmit data
    rfSetAddress(RX_ADDR_P0, address);
}
