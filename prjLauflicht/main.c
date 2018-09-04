#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */



#define LED1_PIN GPIO_PIN_1
#define LED2_PIN GPIO_PIN_0
#define LED3_PIN GPIO_PIN_4
#define LED4_PIN GPIO_PIN_0

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;


uint8_t values = 1;

void interruptHandler() {

    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_INT_PIN_0);
    values++;

    GPIOPinWrite(GPIO_PORTN_BASE, LED1_PIN,0x00);
    GPIOPinWrite(GPIO_PORTN_BASE, LED2_PIN,0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, LED3_PIN,0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, LED4_PIN,0x00);

          switch(values){
          case 1:
              GPIOPinWrite(GPIO_PORTN_BASE, LED1_PIN,0xFF);
              break;
          case 2:
              GPIOPinWrite(GPIO_PORTN_BASE, LED2_PIN,0xFF);
              break;
          case 3:
              GPIOPinWrite(GPIO_PORTF_BASE, LED3_PIN,0xFF);
              break;
          case 4:
              GPIOPinWrite(GPIO_PORTF_BASE, LED4_PIN,0xFF);
              values = 0;
              break;
          default:
              GPIOPinWrite(GPIO_PORTN_BASE, LED1_PIN,0xFF);
              GPIOPinWrite(GPIO_PORTN_BASE, LED2_PIN,0xFF);
              GPIOPinWrite(GPIO_PORTF_BASE, LED3_PIN,0xFF);
              GPIOPinWrite(GPIO_PORTF_BASE, LED4_PIN,0xFF);
              values = 0;
          }
}

void main(void) {

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, LED1_PIN | LED2_PIN);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED3_PIN | LED4_PIN);

    GPIOPinWrite(GPIO_PORTN_BASE, LED1_PIN ,0xFF);

    GPIOIntRegister(GPIO_PORTJ_BASE, interruptHandler);
    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0);

    while(1);
}
