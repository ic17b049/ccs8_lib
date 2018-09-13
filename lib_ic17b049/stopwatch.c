#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */
#include <inc/hw_timer.h> /* Supplies HW Timer */
#include <inc/hw_types.h> /* Supplies HWREG macro */
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <lib_ic17b049/stopwatch.h>

void initStopwatch(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerClockSourceSet(TIMER1_BASE,TIMER_CLOCK_PIOSC);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_A_PERIODIC_UP);
}

void startStopwatch(void){
    TimerEnable(TIMER1_BASE, TIMER_A);
}

void stopStopwatch(void){
    TimerDisable(TIMER1_BASE, TIMER_A);
}

uint32_t getStopwatch(void){
    return TimerValueGet(TIMER1_BASE, TIMER_A)/16;

}

void resetStopwatch(void){
    HWREG(TIMER1_BASE + TIMER_O_TAV) = 0;
}
