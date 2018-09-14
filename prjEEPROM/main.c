#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <driverlib/eeprom.h>
#include <driverlib/sysctl.h>
#include <lib_ic17b049/uart.h>
#include <lib_ic17b049/uartTerminalCMD.h>

//*****************************************************************************
//
// System clock rate in Hz.
//
//*****************************************************************************

uint8_t initEEPROM();
uint8_t writeEEPROM(uint32_t value, uint8_t position);
uint32_t readEEPROM(uint8_t position);

uint32_t g_ui32SysClock;

int main(void)
{
    char sprintstr[50] = "\0";

    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
    SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
    SYSCTL_CFG_VCO_480),
                                        120000000);

    initUART();
    sendTerminalCmdCLS();
    initEEPROM();
    ///writeEEPROM(0x1234, 10);

    snprintf(sprintstr, 50, "0x%X", readEEPROM(10) );
    sendStringToUart(sprintstr);
    sendNewLineToUart();

    return 0;
}

uint8_t initEEPROM()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    uint32_t ui32EEPROMInit = EEPROMInit();

    if (ui32EEPROMInit != EEPROM_INIT_OK)
    {
        return 1;
    }
    return 0;
}

uint8_t writeEEPROM(uint32_t value, uint8_t position)
{
    uint32_t ui32EEPROMProgram = EEPROMProgram(&value, 0x100 + (position * 4),
                                               4);
    if (ui32EEPROMProgram != 0)
    {
        return 1;
    }
    return 0;
}

uint32_t readEEPROM(uint8_t position)
{
    uint32_t pui32Read = NULL;
    EEPROMRead(&pui32Read, 0x100 + (position * 4), 4);
    return pui32Read;
}
