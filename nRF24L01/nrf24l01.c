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
#define CONFIG              0x00
#define EN_AA               0x01
#define ENAA_P0                     0x01
#define EN_RXADDR           0x02
#define ERX_P0                      0x01
#define SETUP_AW            0x03    // Sets the width of the address
#define SETUP_RETR          0x04
#define RF_CH               0x05
#define RF_SETUP            0x06
#define STATUS              0x07
#define RX_P_NO                     0x07
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
#define DPL_P0                      0x01
#define FEATURE             0x1D
#define EN_DPL                      0x04

#define MAX_ADDRESS_LENGTH  4
#define MAX_DATA_BYTES      32

uint8_t rxBuffer[MAX_DATA_BYTES];

void chipEnable()
{
    setPinValue(CHIP_ENABLE, 1);
}

void chipDisable()
{
    setPinValue(CHIP_ENABLE, 0);
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

/*
 * This function sets the address of the rf module.
 * The max address length allowed is 4 bytes.
 * Refer to page 35 of the datasheet to know more about data pipes
 */
void rfSetAddress(uint8_t pipe, uint32_t address)
{
    // The address width is always 4 bytes - 0x10
    rfWriteRegister(SETUP_AW, 0x02);
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

/*
 * Return true if any of the data pipes have data
 */
bool rfIsDataAvailable()
{
    return (((rfReadRegister(STATUS) >> 1) & 7) == 0);
}

void rfSetMode(mode m, uint8_t frequency)
{
    // This is an error as the frequency cannot be more than 2.52GHz
    if(frequency > 125)
        return;
    chipDisable();
    // Frequency = 2400 + RF_CH [MHz]
    rfWriteRegister(RF_CH, frequency);
    switch(m)
    {
    case RX:
        // Appendix B
        // Enable the receive pipe
        rfWriteRegister(EN_RXADDR, ERX_P0);
        // Set a data rate of 1Mbps
        rfWriteRegister(RF_SETUP, 7);
        // Set the payload length for RX in data pipe 0
        // Enable auto acknowledgement on data pipe 0
        rfWriteRegister(EN_AA, ENAA_P0);
        rfWriteRegister(FEATURE, EN_DPL);
        rfWriteRegister(DYNPD, DPL_P0);
        rfWriteRegister(RX_PW_P0, 32);
        // Power up the device and put it in primary receive mode
        rfWriteRegister(CONFIG, 0x70 | PWR_UP | PRIM_RX | EN_CRC);
        chipEnable();
        break;
    case TX:
        // Set reset count 0 to disable auto retransmit
        rfWriteRegister(SETUP_RETR, 0);
        // Set a data rate of 1Mbps
        rfWriteRegister(RF_SETUP, 7);
        // Enable auto acknowledgement on data pipe 0
        rfWriteRegister(EN_AA, ENAA_P0);
        rfWriteRegister(FEATURE, EN_DPL);
        rfWriteRegister(DYNPD, DPL_P0);
        // Power up the device
        rfWriteRegister(CONFIG, 0x70 | PWR_UP | EN_CRC);
        break;
    }
}

void initNrf24l01(uint32_t address)
{
    initSpi1(USE_SSI1_RX);
    selectPinPushPullOutput(CSN);
    selectPinPushPullOutput(CHIP_ENABLE);
    // Run at 8Mbps
    setSpi1BaudRate(5e6, 40e6);
    setSpi1Mode(0, 0);

    // Only use one data pipe to transmit data
    rfSetAddress(RX_ADDR_P0, address);
    rfSetAddress(TX_ADDR, address);
}

void rfReceiveBuffer(uint8_t buffer[], uint8_t nBytes)
{
    rfCsOff();
    writeSpi1Data(R_RX_PAYLOAD);
    readSpi1Data();
    uint8_t i = 0;
    for(i = 0; i < nBytes; i++)
    {
        writeSpi1Data(NOP);
        buffer[i] = readSpi1Data();
    }
    // Clear the receive buffer
    writeSpi1Data(FLUSH_RX);
    readSpi1Data();
    rfCsOn();
}

void rfSendBuffer(uint8_t buffer[], uint8_t nBytes)
{
    // Clear the transmit buffer
    rfCsOff();
    writeSpi1Data(FLUSH_TX);
    readSpi1Data();
    rfCsOn();

    chipDisable();
    rfCsOff();
    writeSpi1Data(W_TX_PAYLOAD);
    readSpi1Data();
    uint8_t i = 0;
    for(i = 0; i < nBytes; i++)
    {
        writeSpi1Data(buffer[i]);
        readSpi1Data();
    }
    rfCsOn();
    rfWriteRegister(STATUS, 0);
    chipEnable();
    _delay_cycles(10000);
    chipDisable();
}
