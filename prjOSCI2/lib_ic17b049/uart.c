#include <stdint.h>
#include <stdbool.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include "lib_ic17b049/uart.h"

static void itoa(uint64_t value, char* result, int base);

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

void sendUint64ToUart(uint64_t value,int base){
    char str[60] = "";
    itoa(value,str,base);
    sendStringToUart(str);
}



void closeUART(){
    UARTDisable(UART0_BASE);
}



void itoa(uint64_t value, char* result, int base){
      // check that the base if valid
      if (base < 2 || base > 36) { *result = '\0';}

      char* ptr = result, *ptr1 = result, tmp_char;
      int tmp_value;

      do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
      } while ( value );

      // Apply negative sign
      if (tmp_value < 0) *ptr++ = '-';
      *ptr-- = '\0';
      while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
      }

    }

