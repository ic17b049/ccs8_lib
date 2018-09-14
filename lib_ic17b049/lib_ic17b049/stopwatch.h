

#ifndef __IC17B049_STOPWATCH_H__
#define __IC17B049_STOPWATCH_H__

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


extern void initStopwatch(void);
extern void startStopwatch(void);
extern void stopStopwatch(void);
extern uint32_t getStopwatch(void);
extern void resetStopwatch(void);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __IC17B049_STOPWATCH_H__
