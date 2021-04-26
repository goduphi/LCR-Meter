/*
 * Sarker Nadir Afridi Azmi
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "gpio.h"
#include "wait.h"
#include "uart0.h"
#include "common_terminal_interface.h"
#include "adc0.h"
#include <stdio.h>

#define INTEGRATE               PORTA,6
#define MEAS_LR                 PORTA,7
#define LOWSIDE_R               PORTB,5
#define HIGHSIDE_R              PORTB,6
#define MEAS_C                  PORTB,7

#define ANALOG_COMPARATOR0      PORTC,7

#define DISCHARGE_TIME          1000000
#define RESISTANCE_CONST        57.90883367
#define CAPACITANCE_CONST       5194551.85
#define INDUCTANCE_CONST        1.5303
#define VSUPPLY                 3.295
#define R33OHMS                 32.7

// PortE masks
#define AIN3                    PORTE,0
#define AIN2                    PORTE,1
#define AIN1                    PORTE,2

// Get the count after compare value in C0 is reached
float chargeTime = 0;
bool isResistanceMeasurement = false;
bool isCapacitanceMeasurement = false;
bool isInductanceMeasurement = false;
char str[50];

void initLcrMeter()
{
    initSystemClockTo40Mhz();

    enablePort(PORTA);
    enablePort(PORTB);
    enablePort(PORTE);

    selectPinPushPullOutput(MEAS_LR);
    selectPinPushPullOutput(INTEGRATE);
    selectPinPushPullOutput(LOWSIDE_R);
    selectPinPushPullOutput(HIGHSIDE_R);
    selectPinPushPullOutput(MEAS_C);

    selectPinAnalogInput(AIN3);
    selectPinAnalogInput(AIN2);
    selectPinAnalogInput(AIN1);
}

void resetMeasurements()
{
    setPinValue(MEAS_LR, 0);
    setPinValue(MEAS_C, 0);
    setPinValue(INTEGRATE, 0);
    setPinValue(LOWSIDE_R, 0);
    setPinValue(HIGHSIDE_R, 0);
}

void initTimer()
{
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
    _delay_cycles(3);

    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off timer before reconfiguring
    TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;           // configure as 32-bit timer (A+B)
    TIMER0_TAMR_R |= TIMER_TAMR_TAMR_1_SHOT | TIMER_TAMR_TACDIR;
}

void initComparator0()
{
    // Provide a clock to the Analog Comparator module
    SYSCTL_RCGCACMP_R |= SYSCTL_RCGCACMP_R0;
    _delay_cycles(3);

    enablePort(PORTC);
    selectPinAnalogInput(ANALOG_COMPARATOR0);

    /*
     * Use the internal reference voltage
     * VIN- > VIN+, VOUT = 0; We want to invert the output so that the interrupt is triggered when the output is 1
     * Trigger interrupt on rising edge and when comparator output is high
     */
    COMP_ACCTL0_R |= COMP_ACCTL0_ASRCP_REF | COMP_ACCTL0_ISEN_RISE | COMP_ACCTL0_CINV;
    // Enable resistor ladder and use a reference voltage of 2.469V
    COMP_ACREFCTL_R |= COMP_ACREFCTL_EN | COMP_ACREFCTL_VREF_M;
    waitMicrosecond(10);
    // Turn off interrupts
    COMP_ACINTEN_R &= ~COMP_ACINTEN_IN0;
    // Vector Number = 41, Interrupt Number = 25
    NVIC_EN0_R |= 1 << (INT_COMP0-16);
}

/*
 * This function returns the resistance of the device under test
 * This only works for small values of R where R < 100 Ohm's
 */
float readDutResistance()
{
    // Read the collector conversion result of Q3 (NPN Transistor)
    setAdc0Ss3Mux(1);
    uint16_t RQ3Vc = readAdc0Ss3();
    // Read the collector conversion result of Q7 (PNP Transistor)
    setAdc0Ss3Mux(2);
    uint16_t RQ7Vc = readAdc0Ss3();
    // Read the conversion result of DUT2
    setAdc0Ss3Mux(3);
    uint16_t Rdut2 = readAdc0Ss3();
    /*
     * Use the voltage divider rule to find small values of R
     * R1 / R2 = V1 / V2
     * V1 = Q7Vc - DUT2
     * V2 = DUT2 - Q3Vc
     * R2 = 32.7 Ohm's
     */
    float q3Vc = (VSUPPLY * (RQ3Vc + 0.5)) / 4096.0;
    float q7Vc = (VSUPPLY * (RQ7Vc + 0.5)) / 4096.0;
    float dut2 = (VSUPPLY * (Rdut2 + 0.5)) / 4096.0;
    float dut2ResistancePd = q7Vc - dut2;
    // Reuse dut2ResistancePd to save stack space
    // This is the esr we are trying to calculate
    dut2ResistancePd = (dut2ResistancePd / (dut2 - q3Vc)) * R33OHMS;
    return dut2ResistancePd;
}

// Records the timer value after the comparator reaches 2.467V
void comparator0Isr()
{
    chargeTime = TIMER0_TAV_R;
    // Clear the interrupt flag
    COMP_ACMIS_R |= COMP_ACMIS_IN0;
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;

    if(isResistanceMeasurement)
    {
        isResistanceMeasurement = false;
        setPinValue(MEAS_LR, 0);
        // Discharge the 1uF capacitor
        setPinValue(INTEGRATE, 1);
        setPinValue(LOWSIDE_R, 1);
        chargeTime /= RESISTANCE_CONST;
        sprintf(str, "Resistance = %.2f Ohm's\n", chargeTime);
        putsUart0(str);
    }

    if(isCapacitanceMeasurement)
    {
        isCapacitanceMeasurement = false;
        setPinValue(HIGHSIDE_R, 0);
        // Discharge capacitor under test
        setPinValue(MEAS_C, 1);
        setPinValue(LOWSIDE_R, 1);
        waitMicrosecond(DISCHARGE_TIME);
        chargeTime /= CAPACITANCE_CONST;
        sprintf(str, "Capacitance = %.2f uF\n", chargeTime);
        putsUart0(str);
    }

    if(isInductanceMeasurement)
    {
        isInductanceMeasurement = false;
        setPinValue(LOWSIDE_R, 0);
        setPinValue(MEAS_LR, 0);

        // Measure the esr
        setPinValue(MEAS_LR, 1);
        setPinValue(LOWSIDE_R, 1);
        waitMicrosecond(1000);
        float esr = readDutResistance();
        setPinValue(MEAS_LR, 0);
        setPinValue(LOWSIDE_R, 0);

        float inductance = ((R33OHMS / (R33OHMS + esr)) * chargeTime) / INDUCTANCE_CONST;
        sprintf(str, "Inductance = %.2f uH\n", inductance);
        putsUart0(str);
    }
}

int main(void)
{
    initLcrMeter();
    initTimer();
    initComparator0();
    initAdc0Ss3();

    // Use AIN3 input with N=4 hardware sampling
    setAdc0Ss3Mux(3);
    setAdc0Ss3Log2AverageCount(2);

    initUart0();
    setUart0BaudRate(115200, 40e6);

    USER_DATA data;

    // Endless loop
    while(true)
    {
        putsUart0("DVM> ");
        getsUart0(&data);
        parseField(&data);
        //COMP_ACINTEN_R &= ~COMP_ACINTEN_IN0;

        // Measure Resistance/Inductance
        if(isCommand(&data, "mlr", 0))
        {
            isResistanceMeasurement = true;
            resetMeasurements();
            // Discharge the 1uF capacitor
            setPinValue(INTEGRATE, 1);
            setPinValue(LOWSIDE_R, 1);
            waitMicrosecond(DISCHARGE_TIME);
            COMP_ACINTEN_R |= COMP_ACINTEN_IN0;
            // Turn of low side r
            setPinValue(LOWSIDE_R, 0);
            // Reset timer value and enable interrupts
            TIMER0_TAV_R = 0;
            setPinValue(MEAS_LR, 1);
            TIMER0_CTL_R |= TIMER_CTL_TAEN;
        }

        // Measure Capacitance
        if(isCommand(&data, "mc", 0))
        {
            isCapacitanceMeasurement = true;
            resetMeasurements();
            // Discharge capacitor under test
            setPinValue(MEAS_C, 1);
            setPinValue(LOWSIDE_R, 1);
            waitMicrosecond(DISCHARGE_TIME);
            setPinValue(LOWSIDE_R, 0);
            COMP_ACINTEN_R |= COMP_ACINTEN_IN0;
            // Reset timer value and enable interrupts
            TIMER0_TAV_R = 0;
            setPinValue(HIGHSIDE_R, 1);
            TIMER0_CTL_R |= TIMER_CTL_TAEN;
        }

        // Measure Resistance/Inductance
        if(isCommand(&data, "mi", 0))
        {
            isInductanceMeasurement = true;
            resetMeasurements();
            setPinValue(LOWSIDE_R, 1);
            // Reset timer value and enable interrupts
            TIMER0_TAV_R = 0;
            COMP_ACINTEN_R |= COMP_ACINTEN_IN0;
            TIMER0_CTL_R |= TIMER_CTL_TAEN;
            setPinValue(MEAS_LR, 1);
        }

        if(isCommand(&data, "v", 0))
        {
            resetMeasurements();
            setPinValue(MEAS_LR, 1);
            setPinValue(LOWSIDE_R, 1);
            waitMicrosecond(1000);
            float esr = readDutResistance();
            sprintf(str, "ESR of device = %.2f Ohm\n", esr);
            putsUart0(str);
            setPinValue(MEAS_LR, 0);
            setPinValue(LOWSIDE_R, 0);
        }
        /*
        // Integrate
        if(isCommand(&data, "int", 1))
        {
            // ~0.1 - 0.2V
            // This part of the circuit discharges the capacitor
            if(stringCompare("low", getFieldString(&data, 1)))
            {
                setPinValue(INTEGRATE, 1);
                setPinValue(LOWSIDE_R, 1);
                setPinValue(HIGHSIDE_R, 0);
            }
            // This part of the circuit charges the capacitor
            else if(stringCompare("int", getFieldString(&data, 1)))
            {
                setPinValue(INTEGRATE, 1);
                setPinValue(LOWSIDE_R, 0);
                setPinValue(HIGHSIDE_R, 1);
            }
        }
        // 3.0 - 3.15
        // Highside
        if(isCommand(&data, "hsr", 0))
        {
            setPinValue(INTEGRATE, 0);
            setPinValue(LOWSIDE_R, 0);
            setPinValue(HIGHSIDE_R, 1);
        }
        // 0.1 - 0.2
        // Lowside
        if(isCommand(&data, "lsr", 0))
        {
            setPinValue(INTEGRATE, 0);
            setPinValue(LOWSIDE_R, 1);
            setPinValue(HIGHSIDE_R, 0);
        }
        */
    }
}
