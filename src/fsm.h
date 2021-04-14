/* 
 * Copyright (c) 2013 Andreas Misje
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef __FSM_H__
#define __FSM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

struct __TruEvent
{
    int nType;
    void *pData;
};

typedef struct __TruEvent TruEvent;

struct __TruState;
typedef struct __TruState TruState;

struct __TruTransition
{
   int nEType;
   void *pCond;
   bool ( *pGuard )( void *pCond, TruEvent *pEvent);
   void ( *pAction )( void *pCSData, TruEvent *pEvent, void *pNSData);
   TruState *pNextState;
};
typedef struct __TruTransition TruTransition;

struct __TruState
{
   TruState *pParent;
   TruState *pEntry;
   TruTransition *pTransitions;
   size_t nNumOfTrans;
   void *pData;
   void (*pEntryAction)(void *pSData, TruEvent *pEvent);
   void (*pExitAction)(void *pSData, TruEvent *pEvent);
};

struct __TruFSM
{
   TruState *pCState;
   TruState *pPState;
   TruState *pEState;
};

typedef struct __TruFSM TruFSM;

enum __EnuSMHandleRet
{
   SM_ERROR_ARG = -2,
   SM_ERROR_STATE_REACHED,
   SM_STATE_CHANGED,
   SM_STATE_LOOP_SELF,
   SM_STATE_NO_STATE_CHANGED,
   SM_STATE_FINAL_STATE_REACHED,
};

typedef enum __EnuSMHandleRet EnuSMHandleRet;

void smInit(TruFSM *pSM, TruState *pIState, TruState *pEState);
int smHandleEvent(TruFSM *pSM, TruEvent *pEvent);
TruState *smGetCState(TruFSM *pSM);
TruState *smGetPState(TruFSM *pSM);
bool smStop(TruFSM *pSM);

#ifdef __cplusplus
}
#endif

#endif //__FSM_H__

