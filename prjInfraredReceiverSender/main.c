#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */

/// PM7,PJ0,PN1
// IR,Switch,LED

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

int main(void) {

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    uint32_t ui32Strength;
       uint32_t ui32PinType;


       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

       GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
       GPIOPadConfigGet(GPIO_PORTJ_BASE, GPIO_PIN_0, &ui32Strength, &ui32PinType);
       GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, ui32Strength, GPIO_PIN_TYPE_STD_WPU);

       GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_2);
       GPIOPadConfigGet(GPIO_PORTD_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
       GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD_WPU);

       GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
       GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_7);

       GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 ,0xFF);
       GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7 ,0x00);



       while(true){
           if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)){
               GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,0x00);
               GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7,0x00);
           }else{
               GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,0xFF);
               GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7,0xFF);
           }
       }
       return 0;
}
