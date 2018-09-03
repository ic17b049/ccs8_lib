#include <stdbool.h>
#include <stdint.h> /* C99 header for uint*_t types */
#include <driverlib/pin_map.h>
#include <driverlib/gpio.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */
#include <driverlib/sysctl.h> /* Supplies SysCtl* functions and SYSCTL_* macros */
#include <driverlib/uart.h>

/**
 * main.c
 */
int main(void)
{
    char asdf[99] = "";
    int temp = 0;
    int i = 0;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTFlowControlSet(UART0_BASE, UART_FLOWCONTROL_NONE );


    UARTConfigSetExpClk(UART0_BASE, 16000000, 38400, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTEnable(UART0_BASE);

    while(asdf[temp-1] != '\r'){
        while(!UARTCharsAvail(UART0_BASE)){
        }

        asdf[temp] = UARTCharGetNonBlocking(UART0_BASE);
        UARTCharPut(UART0_BASE, asdf[temp]);
        temp++;
    }

    for(i = 0; i<temp;i++){
        UARTCharPut(UART0_BASE, asdf[i]);
    }


    UARTDisable(UART0_BASE);

    return 0;
}
