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

int main(void)
{
    initSystemClockTo40Mhz();

    initNrf24l01(0xDEEBA);

    initUart0();
    setUart0BaudRate(115200, 40e6);

    USER_DATA data;

	while(true)
	{
	    getsUart0(&data);
	    parseField(&data);

	    if(isCommand(&data, "status", 0))
	    {
	        uint8_t buffer[4];
	        rfReadIntoBuffer(0x0A, buffer, 4);
	        char out[50];
	        sprintf(out, "Current Address = %x%x%x%x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
	        putsUart0(out);
	    }
	}
}
