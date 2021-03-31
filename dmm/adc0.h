// ADC0  Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// ADC0 SS3

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef ADC0_H_
#define ADC0_H_

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initAdc0Ss3();
void setAdc0Ss3Log2AverageCount(uint8_t log2AverageCount);
void setAdc0Ss3Mux(uint8_t input);
int16_t readAdc0Ss3();

#endif
