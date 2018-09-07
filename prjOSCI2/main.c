#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */
#include <inc/hw_timer.h> /* Supplies HW Timer */
#include "inc/hw_ints.h"
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/timer.h>
#include <driverlib/interrupt.h>
#include <lib_ic17b049/uart.h>
#include <lib_ic17b049/stopwatch.h>
#include <lib_ic17b049/uartTerminalCMD.h>






//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
enum { Lead = 0x01, LeadSpace = 0x02, BitStart = 0x04, Bit0 = 0x08, Bit1 = 0x10, DataSpace = 0x20, SINGLEBIT = 0x40, SINGLEBITSPACE =  0x80};

uint32_t g_ui32SysClock;
uint8_t irSigNr = 0;
uint8_t irExpNext = Lead;
uint8_t irBitsReceived = 0;
uint8_t irData = 0;
uint8_t irDataInv = 0;
uint8_t irAddress = 0;
uint8_t irAddressInv = 0;




void interruptHandlerIrInit(void);
void interruptHandlerIr(void);

void restart(void);
void irBitReceived(bool data);

void main(void) {

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);


    initUART();
    sendTerminalCmdCLS();

    initStopwatch();

    TimerLoadSet(TIMER1_BASE, TIMER_A, 16000000/4);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER1_BASE,TIMER_A,restart);


    uint32_t ui32Strength;
    uint32_t ui32PinType;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPadConfigGet(GPIO_PORTD_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD_WPU);



    GPIOIntRegister(GPIO_PORTD_BASE, interruptHandlerIrInit);
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_2);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    while(1);
}


void interruptHandlerIrInit(){
    GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_2);
    irSigNr = 0;
    irExpNext = Lead;
    resetStopwatch();
    startStopwatch();

    GPIOIntRegister(GPIO_PORTD_BASE, interruptHandlerIr);
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_RISING_EDGE);
}


void interruptHandlerIr(){
    GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_2);
    uint32_t time = getStopwatch();
    resetStopwatch();
    bool irFailure = true;
    irSigNr++;


    if((irExpNext & Lead) && irFailure == true){
        if(time > 9000-1000 && time < 9000+1000 && ( irSigNr == 1||irSigNr == 69||irSigNr == 5 )){
                irFailure = false;
                irExpNext = LeadSpace;

                irSigNr = 1;
                irBitsReceived = 0;
                irData = 0;
                irDataInv = 0;
                irAddress = 0;
                irAddressInv = 0;
            }
    }

    if((irExpNext & LeadSpace) && irFailure == true){
        if(time > 4500-1000 && time < 4500+1000 && irSigNr == 2){
                irFailure = false;
                irExpNext = BitStart;
            }
        if(time > 2250-250 && time < 2250+250 && irSigNr == 2){
                irFailure = false;
                irExpNext = SINGLEBIT;
            }
    }

    if((irExpNext & SINGLEBIT) && irFailure == true && irSigNr == 3){
        if(time > 550-100 && time < 550+100){
            irFailure = false;
            irExpNext = Bit0|Bit1;
            irExpNext = SINGLEBITSPACE;
        }
    }


    if((irExpNext & SINGLEBITSPACE) && irFailure == true && irSigNr == 4){
        if(time > 98000-10000 && time < 98000+10000){
            irFailure = false;
            irExpNext = Lead;
        }
    }


    if((irExpNext & BitStart) && irFailure == true){
        if(time > 550-100 && time < 550+100 && (irSigNr-3)%2 == 0){
                irFailure = false;

                if(irBitsReceived != 32){
                    irExpNext = Bit0|Bit1;
                }else{
                    irExpNext = DataSpace;
                }
            }
    }


    if((irExpNext & Bit0) && irFailure == true){
        if(time > 1500-500 && time < 1500+500 && (irSigNr-3)%2 == 1){
                irFailure = false;
                irBitsReceived++;
                irBitReceived(0);
                irExpNext = BitStart;
            }
    }


    if((irExpNext & Bit1) && irFailure == true){
        if(time > 550-100 && time < 550+100 && (irSigNr-3)%2 == 1){
                irFailure = false;
                irBitsReceived++;
                irBitReceived(1);
                irExpNext = BitStart;
            }
    }

    if((irExpNext & DataSpace) && irFailure == true){
        if(time > 40000-10000 && time < 40000+10000 && irSigNr == 68){
                irFailure = false;


                if((irData&irDataInv) != 0x00 || (irAddress&irAddressInv) != 0x00){
                    sendStringToUart("PFZFehler");
                }

                sendUint64ToUart(irAddressInv, 16);
                sendUint64ToUart(irDataInv, 16);

                irExpNext = Lead;
            }
    }


    if(irFailure == true){
        sendNewLineToUart();
        sendUint64ToUart(time, 10);
        sendStringToUart(" ");
        sendUint64ToUart(irSigNr, 10);

    }


    if(irSigNr%2 == 0){
        GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_RISING_EDGE);
    }else{
        GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);
    }

}


void restart(){
    sendNewLineToUart();
    sendStringToUart("EOL");
    sendNewLineToUart();


    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    stopStopwatch();
    GPIOIntRegister(GPIO_PORTD_BASE, interruptHandlerIrInit);
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);

}



void irBitReceived(bool data){
    uint8_t mask = data? 0xFF:0x00;


    if(irBitsReceived<=8){
        irAddress |= ((0x01<<(irBitsReceived-1)) & mask);
    }else if(irBitsReceived<=16){
        irAddressInv |= ((0x01<<(irBitsReceived-9)) & mask);
    }else if(irBitsReceived<=24){
        irData |= ((0x01<<(irBitsReceived-17)) & mask);
    }else if(irBitsReceived<=32){
        irDataInv |= ((0x01<<(irBitsReceived-25)) & mask);
    }
}

