/*
 * nrf24l01.h
 *
 *  Created on: Mar 25, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef NRF24L01_H_
#define NRF24L01_H_

// Commands
#define R_REGISTER          0x00
#define W_REGISTER          0x20
#define R_RX_PAYLOAD        0x61
#define W_TX_PAYLOAD        0xA0
#define FLUSH_TX            0xE1
#define FLUSH_RX            0xE2
#define W_TX_PAYLOAD_NO_ACK 0xB0
#define NOP                 0xFF

// Register bit fields
#define EN_CRC              8
#define PWR_UP              2
#define PRIM_RX             1

typedef enum _mode
{
    POWER_DOWN, RX, TX
} mode;

void initNrf24l01(uint32_t address);
void rfWriteRegister(uint8_t reg, uint8_t data);
uint8_t rfReadRegister(uint8_t reg);
void rfReadIntoBuffer(uint8_t reg, uint8_t buffer[], uint8_t nBytes);
void rfSendBuffer(uint8_t buffer[], uint8_t nBytes);
void rfSetMode(mode m, uint8_t frequency);
bool rfIsDataAvailable();
void rfReceiveBuffer(uint8_t buffer[], uint8_t nBytes);

#endif /* NRF24L01_H_ */
