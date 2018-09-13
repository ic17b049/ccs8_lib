#include <stdint.h>
#include <stdbool.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <inc/hw_memmap.h> /* Supplies GPIO_PORTx_BASE */
#include "inc/hw_ints.h"
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/interrupt.h>
#include "lib_ic17b049/uart.h"

#define sample 5000

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;
char zeichen = '0';

uint32_t varTime[sample];
bool varValue[sample];
uint16_t tmpTimer = 0;

uint16_t pTime[100];
bool pValue[100];
uint16_t tmpPValues = 0;

void printTimeData(uint32_t time, bool value);
void processData(uint16_t *time, bool *value);
void interruptHandler(void);
void itoa(uint64_t value, char* result, int base)
    {
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




void
Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    varTime[tmpTimer] = TimerValueGet64(TIMER1_BASE);
    varValue[tmpTimer] = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2);

    tmpTimer++;


    if(tmpTimer == sample){
        int i = 0;
        bool cVal = 0;
        uint32_t cTime = varTime[0];

        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 ,0x00);

        for(i=0;i<sample;i++){

            //printTimeData(varTime[i], varValue[i]);
            varValue[i] = !varValue[i];
            varTime[i] = varTime[i] /  16.4475555555556;
        }

        for(i=0;i<sample;i++){
           if(cVal != varValue[i]){

               printTimeData(varTime[i]-cTime, cVal);

               pTime[tmpPValues] = varTime[i]-cTime;
               pValue[tmpPValues] = cVal;
               tmpPValues++;



               cVal = varValue[i];
               cTime = varTime[i];
           }

           pTime[tmpPValues] = 0;
           pValue[tmpPValues] = 0;

        }

        processData(&(pTime[1]), &(pValue[1]));
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 ,0x00);

        GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_2);
        tmpPValues = 0;

        SysCtlDelay(g_ui32SysClock/4);
        SysCtlReset();

    }
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}



void processData(uint16_t *time, bool *value){
    uint16_t data = 0;
    uint16_t datainv = 0;
    uint16_t accuracy = 100;
    uint8_t tmpVal = 0;

    if(time[tmpVal] < 9000 - accuracy*3 || time[0] > 9000 + accuracy*3 || value[tmpVal] == 0){
        sendStringToUart("ProtokollFehler Leadercode\r\n");
        exit(1); //ProtokollFehler Leadercode
    }
    tmpVal++;
    if(time[tmpVal] < 4500 - accuracy*3 || time[tmpVal] > 4500 + accuracy*3 || value[tmpVal] == 1){
        sendStringToUart("ProtokollFehler Space\r\n");
        exit(1); //ProtokollFehler Leadercode Space
    }
    tmpVal++;
    uint8_t i;
    for(i = 0;i<8;i++){


        if(time[tmpVal] < 550 - accuracy || time[tmpVal] > 550 + accuracy || value[tmpVal] == 0){
            sendStringToUart("ProtokollFehler Bit1\r\n");
            exit(1); //ProtokollFehler Bit
        }
        tmpVal++;
        if(time[tmpVal] > 550 - accuracy && time[tmpVal] < 550 + accuracy){
            //0 Bit
            data = data>>1;
            data = data|0x0000;
        }else if(time[tmpVal] > 1650 - accuracy && time[tmpVal] < 1650 + accuracy){
            //1 Bit
            data = data>>1;
            data = data|0x8000;
        }else{
            sendStringToUart("ProtokollFehler Bit Space\r\n");
            exit(1); //ProtokollFehler Bit Spcae
        }
        tmpVal++;
    }


    for(i = 0;i<8;i++){


           if(time[tmpVal] < 550 - accuracy || time[tmpVal] > 550 + accuracy || value[tmpVal] == 0){
               sendStringToUart("ProtokollFehler Bit2\r\n");
               exit(1); //ProtokollFehler Bit
           }
           tmpVal++;
           if(time[tmpVal] > 550 - accuracy && time[tmpVal] < 550 + accuracy){
               //0 Bit
               datainv = datainv>>1;
               datainv = datainv|0x0000;
           }else if(time[tmpVal] > 1650 - accuracy && time[tmpVal] < 1650 + accuracy){
               //1 Bit
               datainv = datainv>>1;
               datainv = datainv|0x8000;
           }else{
               sendStringToUart("ProtokollFehler Bit Space\r\n");
               exit(1); //ProtokollFehler Bit Spcae
           }
           tmpVal++;
       }

    for(i = 0;i<8;i++){


        if(time[tmpVal] < 550 - accuracy || time[tmpVal] > 550 + accuracy || value[tmpVal] == 0){
            sendStringToUart("ProtokollFehler Bit3\r\n");
            exit(1); //ProtokollFehler Bit
        }
        tmpVal++;
        if(time[tmpVal] > 550 - accuracy && time[tmpVal] < 550 + accuracy){
            //0 Bit
            data = data>>1;
            data = data|0x0000;
        }else if(time[tmpVal] > 1650 - accuracy && time[tmpVal] < 1650 + accuracy){
            //1 Bit
            data = data>>1;
            data = data|0x8000;
        }else{
            sendStringToUart("ProtokollFehler Bit Space\r\n");
            exit(1); //ProtokollFehler Bit Spcae
        }
        tmpVal++;
    }


    for(i = 0;i<8;i++){


           if(time[tmpVal] < 550 - accuracy || time[tmpVal] > 550 + accuracy || value[tmpVal] == 0){
               sendStringToUart("ProtokollFehler Bit4\r\n");
               exit(1); //ProtokollFehler Bit
           }
           tmpVal++;
           if(time[tmpVal] > 550 - accuracy && time[tmpVal] < 550 + accuracy){
               //0 Bit
               datainv = datainv<<1;
               datainv = datainv|0x0000;
           }else if(time[tmpVal] > 1650 - accuracy && time[tmpVal] < 1650 + accuracy){
               //1 Bit
               datainv = datainv<<1;
               datainv = datainv|0x8000;
           }else{
               sendStringToUart("ProtokollFehler Bit Space\r\n");
               exit(1); //ProtokollFehler Bit Spcae
           }
           tmpVal++;
       }


    uint16_t data2 = data;
data2 = data2>>8;
data = data <<8;

data = data|data2;



    char str[60] = "";

    switch(data) {
        case 0x0408: sendStringToUart("ON/OFF"); break;
        case 0x0410: sendStringToUart("0"); break;
        case 0x0411: sendStringToUart("1"); break;
        case 0x0412: sendStringToUart("2"); break;
        case 0x0413: sendStringToUart("3"); break;
        case 0x0414: sendStringToUart("4"); break;
        case 0x0415: sendStringToUart("5"); break;
        case 0x0416: sendStringToUart("6"); break;
        case 0x0417: sendStringToUart("7"); break;
        case 0x0418: sendStringToUart("8"); break;
        case 0x0419: sendStringToUart("9"); break;
        default:

            sendStringToUart("New Signal: 0x0");
            itoa(data,str,16);
            sendStringToUart(str);
            sendStringToUart("\r\n");

            break;
    }

}



void interruptHandler() {
    GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_2);
    GPIOIntDisable(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 ,0xFF);
    TimerEnable(TIMER1_BASE, TIMER_A);
    TimerEnable(TIMER0_BASE, TIMER_A);
}


void printTimeData(uint32_t time, bool value) {
            char str[60] = "";

            itoa(time,str,10);
            sendStringToUart(str);
            sendStringToUart(" ");
            itoa(value,str,10);
            sendStringToUart(str);
            sendStringToUart("\r\n");
}

void main(void) {

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);

    initUART();

    uint32_t ui32Strength;
    uint32_t ui32PinType;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPadConfigGet(GPIO_PORTD_BASE, GPIO_PIN_2, &ui32Strength, &ui32PinType);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2, ui32Strength, GPIO_PIN_TYPE_STD_WPU);





    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);




    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerClockSourceSet(TIMER1_BASE,TIMER_CLOCK_PIOSC);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC_UP);
    TimerLoadSet64(TIMER1_BASE, 18446744073709551615);



    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock/20000);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0IntHandler);






    GPIOIntRegister(GPIO_PORTD_BASE, interruptHandler);
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_2);





    while(1);

    closeUART();



}
