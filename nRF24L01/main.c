/**
 * Sarker Nadir Afridi Azmi
 */

#include <stdint.h>
#include <stdbool.h>
#include "clock.h"
#include "gpio.h"
#include "spi1.h"
#include "uart0.h"
#include "common_terminal_interface.h"
#include "nrf24l01.h"
#include <stdio.h>

//#define MODE

int main(void)
{
    initSystemClockTo40Mhz();

    initNrf24l01(0xACCE55);
    rfSetMode(RX, 20);

    initUart0();
    setUart0BaudRate(115200, 40e6);

    USER_DATA data;

    putsUart0("This board is in RX mode\n");

    char out[50];
	while(true)
	{
#ifdef MODE
	    getsUart0(&data);
	    parseField(&data);
	    if(isCommand(&data, "status", 0))
	    {
	        sprintf(out, "Status = %d\n", rfReadRegister(getInteger(&data, data.fieldPosition[1])));
	        putsUart0(out);
	    }
	    if(isCommand(&data, "addr", 0))
	    {
	        uint8_t buffer[4];
            rfReadIntoBuffer(0x0A, buffer, 4);
            sprintf(out, "RX = %x-%x-%x-%x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
            putsUart0(out);
            rfReadIntoBuffer(0x10, buffer, 4);
            sprintf(out, "TX = %x-%x-%x-%x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
            putsUart0(out);
	    }
	    if(isCommand(&data, "send", 0))
	    {
	        uint8_t buffer[4];
	        uint8_t i = 0;
	        for(i = 0; i < data.fieldCount; i++)
	        {
	            buffer[i] = getInteger(&data, data.fieldPosition[1 + i]);
	        }
	        rfSendBuffer(buffer, 4);
	    }
#endif
#ifndef MODE
	    if(rfIsDataAvailable())
	    {
	        uint8_t buffer[4];
	        putsUart0("There is data in the receive FIFO\n");
	        rfReceiveBuffer(buffer, 4);
	        sprintf(out, "RX = %x-%x-%x-%x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
            putsUart0(out);
	    }
#endif
	}
}
