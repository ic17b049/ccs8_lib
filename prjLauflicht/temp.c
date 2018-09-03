#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */

#define USERSWITCH_PIN GPIO_PIN_0
//#define USERSWITCH_PIN GPIO_PIN_1

#define USERSWITCH_BASE SYSCTL_PERIPH_GPIOJ

#define LED1_PIN GPIO_PIN_1
#define LED2_PIN GPIO_PIN_0
#define LED3_PIN GPIO_PIN_4
#define LED4_PIN GPIO_PIN_0

#define LED1_LED2_BASE SYSCTL_PERIPH_GPION
#define LED3_LED4_BASE SYSCTL_PERIPH_GPIOF

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************


int main(void) {
    uint8_t values = 1;
    uint32_t ui32Strength;
    uint32_t ui32PinType;


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);

    GPIOPadConfigGet(GPIO_PORTJ_BASE, 0, &ui32Strength, &ui32PinType);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, ui32Strength, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, LED1_PIN | LED2_PIN);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED3_PIN | LED4_PIN);

    GPIOPinWrite(GPIO_PORTN_BASE, LED1_PIN ,0xFF);


    while(true){

        while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) && 0xFF){
        }



        GPIOPinWrite(GPIO_PORTN_BASE, LED1_PIN,0);
        GPIOPinWrite(GPIO_PORTN_BASE, LED2_PIN,0);
        GPIOPinWrite(GPIO_PORTF_BASE, LED3_PIN,0);
        GPIOPinWrite(GPIO_PORTF_BASE, LED4_PIN,0);

        values++;

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
        }

        while(!(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) && 0xFF)){
        }


    }
    return 0;
}
