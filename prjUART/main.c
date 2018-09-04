#include <stdint.h>
#include <stdbool.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;
char zeichen = '0';

void interruptHandler() {
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_INT_PIN_0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 ,0xFF);


    while(!UARTCharsAvail(UART0_BASE)){
    }

    zeichen = UARTCharGetNonBlocking(UART0_BASE);
    UARTCharPut(UART0_BASE, zeichen);


}

void main(void) {

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);



      SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
      SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

      GPIOPinConfigure(GPIO_PA0_U0RX);
      GPIOPinConfigure(GPIO_PA1_U0TX);

      GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

      UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
      UARTFlowControlSet(UART0_BASE, UART_FLOWCONTROL_NONE );

      UARTConfigSetExpClk(UART0_BASE, 16000000, 38400, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

      UARTEnable(UART0_BASE);

      UARTIntRegister(UART0_BASE,interruptHandler);
      UARTFIFODisable(UART0_BASE);
      UARTIntEnable(UART0_BASE,UART_INT_TX|UART_INT_RX);

      while(zeichen != '\r'){
      }



      UARTDisable(UART0_BASE);

}
