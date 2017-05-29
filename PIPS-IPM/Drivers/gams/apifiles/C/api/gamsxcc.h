/* gamsxcc.h
 * Header file for C-style interface to the GAMSX library
 * generated by apiwrapper for GAMS Version 24.8.3
 * Copyright (c) 2016, GAMS Development Corp.
 */

#if ! defined(_GAMSXCC_H_)
#     define  _GAMSXCC_H_

#define GAMSXAPIVERSION 1



#include "gclgms.h"

#if defined(_WIN32)
# define GAMSX_CALLCONV __stdcall
#else
# define GAMSX_CALLCONV
#endif

#if defined(__cplusplus)
extern "C" {
#endif

struct gamsxRec;
typedef struct gamsxRec *gamsxHandle_t;

typedef int (*gamsxErrorCallback_t) (int ErrCount, const char *msg);

/* headers for "wrapper" routines implemented in C */
int gamsxGetReady  (char *msgBuf, int msgBufLen);
int gamsxGetReadyD (const char *dirName, char *msgBuf, int msgBufLen);
int gamsxGetReadyL (const char *libName, char *msgBuf, int msgBufLen);
int gamsxCreate    (gamsxHandle_t *pgamsx, char *msgBuf, int msgBufLen);
int gamsxCreateD   (gamsxHandle_t *pgamsx, const char *dirName, char *msgBuf, int msgBufLen);
int gamsxCreateL   (gamsxHandle_t *pgamsx, const char *libName, char *msgBuf, int msgBufLen);
int gamsxFree      (gamsxHandle_t *pgamsx);

int gamsxLibraryLoaded(void);
int gamsxLibraryUnload(void);

int  gamsxGetScreenIndicator   (void);
void gamsxSetScreenIndicator   (int scrind);
int  gamsxGetExceptionIndicator(void);
void gamsxSetExceptionIndicator(int excind);
int  gamsxGetExitIndicator     (void);
void gamsxSetExitIndicator     (int extind);
gamsxErrorCallback_t gamsxGetErrorCallback(void);
void gamsxSetErrorCallback(gamsxErrorCallback_t func);
int  gamsxGetAPIErrorCount     (void);
void gamsxSetAPIErrorCount     (int ecnt);

void gamsxErrorHandling(const char *msg);
void gamsxInitMutexes(void);
void gamsxFiniMutexes(void);


#if defined(GAMSX_MAIN)    /* we must define some things only once */
# define GAMSX_FUNCPTR(NAME)  NAME##_t NAME = NULL
#else
# define GAMSX_FUNCPTR(NAME)  extern NAME##_t NAME
#endif

/* function typedefs and pointer definitions */

typedef int (GAMSX_CALLCONV *TBrkPCallBack1_t) (const char *fn, int linNr, int lstNr, void *usrMem);
typedef int (GAMSX_CALLCONV *TBrkPCallBack2_t) (const char *fn, int linNr, int lstNr, void *userMem1, void *userMem2);

/* Prototypes for Dummy Functions */
int  GAMSX_CALLCONV d_gamsxRunExecDLL (gamsxHandle_t pgamsx, void *optPtr, const char *sysDir, int AVerbose, char *Msg);
int  GAMSX_CALLCONV d_gamsxShowError (gamsxHandle_t pgamsx, const char *fNameLog, char *errorLine, char *errorTyp, char *gmsLine);
void  GAMSX_CALLCONV d_gamsxAddBreakPoint (gamsxHandle_t pgamsx, const char *fn, int lineNr);
void  GAMSX_CALLCONV d_gamsxClearBreakPoints (gamsxHandle_t pgamsx);
int  GAMSX_CALLCONV d_gamsxSystemInfo (gamsxHandle_t pgamsx, int *NrSy, int *NrUel);
int  GAMSX_CALLCONV d_gamsxSymbolInfo (gamsxHandle_t pgamsx, int SyNr, char *SyName, char *SyExplTxt, int *SyDim, int *SyTyp, int *SyCount, int *SyUserInfo);
char * GAMSX_CALLCONV d_gamsxUelName (gamsxHandle_t pgamsx, int uel, char *buf);
int  GAMSX_CALLCONV d_gamsxFindSymbol (gamsxHandle_t pgamsx, const char *SyName);
int  GAMSX_CALLCONV d_gamsxDataReadRawStart (gamsxHandle_t pgamsx, int SyNr, int *SyCount);
int  GAMSX_CALLCONV d_gamsxDataReadRaw (gamsxHandle_t pgamsx, int Elements[], double Vals[], int *FDim);
int  GAMSX_CALLCONV d_gamsxDataReadDone (gamsxHandle_t pgamsx);
int  GAMSX_CALLCONV d_gamsxDataWriteRawStart (gamsxHandle_t pgamsx, int SyNr, int DoMerge);
int  GAMSX_CALLCONV d_gamsxDataWriteRaw (gamsxHandle_t pgamsx, const int Elements[], const double Vals[]);
int  GAMSX_CALLCONV d_gamsxDataWriteDone (gamsxHandle_t pgamsx);
void  GAMSX_CALLCONV d_gamsxRegisterCB1 (gamsxHandle_t pgamsx, TBrkPCallBack1_t CB1, void *userMem);
void  GAMSX_CALLCONV d_gamsxRegisterCB2 (gamsxHandle_t pgamsx, TBrkPCallBack2_t CB2, void *userMem1, void *userMem2);
TBrkPCallBack1_t GAMSX_CALLCONV d_gamsxGetCB1 (gamsxHandle_t pgamsx);
TBrkPCallBack2_t GAMSX_CALLCONV d_gamsxGetCB2 (gamsxHandle_t pgamsx);
void * GAMSX_CALLCONV d_gamsxGetCB1UM (gamsxHandle_t pgamsx);
void * GAMSX_CALLCONV d_gamsxGetCB2UM1 (gamsxHandle_t pgamsx);
void * GAMSX_CALLCONV d_gamsxGetCB2UM2 (gamsxHandle_t pgamsx);
void GAMSX_CALLCONV d_gamsxSWSet (gamsxHandle_t pgamsx, const int x);
int  GAMSX_CALLCONV d_gamsxStepThrough (gamsxHandle_t pgamsx);
void GAMSX_CALLCONV d_gamsxStepThroughSet (gamsxHandle_t pgamsx, const int x);
int  GAMSX_CALLCONV d_gamsxRunToEnd (gamsxHandle_t pgamsx);
void GAMSX_CALLCONV d_gamsxRunToEndSet (gamsxHandle_t pgamsx, const int x);
int  GAMSX_CALLCONV d_gamsxCB1Defined (gamsxHandle_t pgamsx);
int  GAMSX_CALLCONV d_gamsxCB2Defined (gamsxHandle_t pgamsx);

typedef int  (GAMSX_CALLCONV *gamsxRunExecDLL_t) (gamsxHandle_t pgamsx, void *optPtr, const char *sysDir, int AVerbose, char *Msg);
GAMSX_FUNCPTR(gamsxRunExecDLL);
typedef int  (GAMSX_CALLCONV *gamsxShowError_t) (gamsxHandle_t pgamsx, const char *fNameLog, char *errorLine, char *errorTyp, char *gmsLine);
GAMSX_FUNCPTR(gamsxShowError);
typedef void  (GAMSX_CALLCONV *gamsxAddBreakPoint_t) (gamsxHandle_t pgamsx, const char *fn, int lineNr);
GAMSX_FUNCPTR(gamsxAddBreakPoint);
typedef void  (GAMSX_CALLCONV *gamsxClearBreakPoints_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxClearBreakPoints);
typedef int  (GAMSX_CALLCONV *gamsxSystemInfo_t) (gamsxHandle_t pgamsx, int *NrSy, int *NrUel);
GAMSX_FUNCPTR(gamsxSystemInfo);
typedef int  (GAMSX_CALLCONV *gamsxSymbolInfo_t) (gamsxHandle_t pgamsx, int SyNr, char *SyName, char *SyExplTxt, int *SyDim, int *SyTyp, int *SyCount, int *SyUserInfo);
GAMSX_FUNCPTR(gamsxSymbolInfo);
typedef char * (GAMSX_CALLCONV *gamsxUelName_t) (gamsxHandle_t pgamsx, int uel, char *buf);
GAMSX_FUNCPTR(gamsxUelName);
typedef int  (GAMSX_CALLCONV *gamsxFindSymbol_t) (gamsxHandle_t pgamsx, const char *SyName);
GAMSX_FUNCPTR(gamsxFindSymbol);
typedef int  (GAMSX_CALLCONV *gamsxDataReadRawStart_t) (gamsxHandle_t pgamsx, int SyNr, int *SyCount);
GAMSX_FUNCPTR(gamsxDataReadRawStart);
typedef int  (GAMSX_CALLCONV *gamsxDataReadRaw_t) (gamsxHandle_t pgamsx, int Elements[], double Vals[], int *FDim);
GAMSX_FUNCPTR(gamsxDataReadRaw);
typedef int  (GAMSX_CALLCONV *gamsxDataReadDone_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxDataReadDone);
typedef int  (GAMSX_CALLCONV *gamsxDataWriteRawStart_t) (gamsxHandle_t pgamsx, int SyNr, int DoMerge);
GAMSX_FUNCPTR(gamsxDataWriteRawStart);
typedef int  (GAMSX_CALLCONV *gamsxDataWriteRaw_t) (gamsxHandle_t pgamsx, const int Elements[], const double Vals[]);
GAMSX_FUNCPTR(gamsxDataWriteRaw);
typedef int  (GAMSX_CALLCONV *gamsxDataWriteDone_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxDataWriteDone);
typedef void  (GAMSX_CALLCONV *gamsxRegisterCB1_t) (gamsxHandle_t pgamsx, TBrkPCallBack1_t CB1, void *userMem);
GAMSX_FUNCPTR(gamsxRegisterCB1);
typedef void  (GAMSX_CALLCONV *gamsxRegisterCB2_t) (gamsxHandle_t pgamsx, TBrkPCallBack2_t CB2, void *userMem1, void *userMem2);
GAMSX_FUNCPTR(gamsxRegisterCB2);
typedef TBrkPCallBack1_t (GAMSX_CALLCONV *gamsxGetCB1_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxGetCB1);
typedef TBrkPCallBack2_t (GAMSX_CALLCONV *gamsxGetCB2_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxGetCB2);
typedef void * (GAMSX_CALLCONV *gamsxGetCB1UM_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxGetCB1UM);
typedef void * (GAMSX_CALLCONV *gamsxGetCB2UM1_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxGetCB2UM1);
typedef void * (GAMSX_CALLCONV *gamsxGetCB2UM2_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxGetCB2UM2);
typedef void (GAMSX_CALLCONV *gamsxSWSet_t) (gamsxHandle_t pgamsx, const int x);
GAMSX_FUNCPTR(gamsxSWSet);
typedef int  (GAMSX_CALLCONV *gamsxStepThrough_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxStepThrough);
typedef void (GAMSX_CALLCONV *gamsxStepThroughSet_t) (gamsxHandle_t pgamsx, const int x);
GAMSX_FUNCPTR(gamsxStepThroughSet);
typedef int  (GAMSX_CALLCONV *gamsxRunToEnd_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxRunToEnd);
typedef void (GAMSX_CALLCONV *gamsxRunToEndSet_t) (gamsxHandle_t pgamsx, const int x);
GAMSX_FUNCPTR(gamsxRunToEndSet);
typedef int  (GAMSX_CALLCONV *gamsxCB1Defined_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxCB1Defined);
typedef int  (GAMSX_CALLCONV *gamsxCB2Defined_t) (gamsxHandle_t pgamsx);
GAMSX_FUNCPTR(gamsxCB2Defined);
#if defined(__cplusplus)
}
#endif
#endif /* #if ! defined(_GAMSXCC_H_) */
