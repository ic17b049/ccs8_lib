#include "lib_ic17b049/uart.h"
#include "lib_ic17b049/uartTerminalCMD.h"


void sendTerminalCmdCLS(void){

    sendCharToUart(27);
    sendStringToUart("[2J");
    sendCharToUart(27);
    sendStringToUart("[H");
    sendCharToUart(27);
    sendStringToUart("[3J");


}
