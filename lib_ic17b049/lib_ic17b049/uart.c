#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include "lib_ic17b049/uart.h"

void initUART(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTFlowControlSet(UART0_BASE, UART_FLOWCONTROL_NONE );

    UARTConfigSetExpClk(UART0_BASE, 16000000, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTEnable(UART0_BASE);
}


void sendStringToUart(char* string){

    while(*string != '\0'){
        UARTCharPut(UART0_BASE, *string);
        string++;
    }
}

void sendCharToUart(char ch){
        UARTCharPut(UART0_BASE, ch);
}

void sendNewLineToUart(){
    sendStringToUart("\r\n");
}

void closeUART(){
    UARTDisable(UART0_BASE);
}

