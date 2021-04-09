/*
 * spi1.h
 *
 *  Created on: Mar 26, 2021
 *      Author: afrid
 */

#ifndef SPI1_H_
#define SPI1_H_

#include <stdint.h>
#include <stdbool.h>

#define USE_SSI1_FSS 1
#define USE_SSI1_RX  2

void initSpi1(uint32_t pinMask);
void setSpi1BaudRate(uint32_t clockRate, uint32_t fcyc);
void setSpi1Mode(uint8_t polarity, uint8_t phase);
void writeSpi1Data(uint32_t data);
uint32_t readSpi1Data();

#endif /* SPI1_H_ */
