#include <stdint.h>

#ifndef __IC17B049_UART_H__
#define __IC17B049_UART_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


extern void initUART(void);
extern void sendCharToUart(char ch);
extern void sendStringToUart(char *string);
extern void sendNewLineToUart();
extern void sendUint64ToUart(uint64_t value,int base);
extern void closeUART();



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __IC17B049_UART_H__
