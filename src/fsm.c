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

#include "fsm.h"
#include <stdint.h>
#include <assert.h>
#include <string.h>

static void goToErrorState( TruFSM *fsm,
      TruEvent *const event );
static TruTransition *getTransition( TruFSM *fsm,
      TruState *state, TruEvent *const event );

void smInit( TruFSM *fsm,
      TruState *initialState, TruState *errorState )
{
   if ( !fsm )
      return;

   fsm->pCState = initialState;
   fsm->pPState = NULL;
   fsm->pEState = errorState;
   
   fsm->pCState->pEntryAction( fsm->pCState->pData, NULL );
}

static TruState **stateM_getTrasitionPath(TruState *pCurrentState, TruState *pNextState)
{
    static TruState *lptrv_Buf[20];
    TruState *lptrv_CurrentStatePath[10];
    TruState *lptrv_NextStatePath[10];
    int32_t ls32v_CurrentStateCnt = 0;
    int32_t ls32v_NextStateCnt = 0;
    
    /* Check Input */
    if((!pCurrentState) || (!pNextState))
    {
        //error(" ");
        return NULL;
    }
    
    /* get path */
    ls32v_CurrentStateCnt = 0;
    while(pCurrentState && (ls32v_CurrentStateCnt<10))
    {
        lptrv_CurrentStatePath[ls32v_CurrentStateCnt++] = pCurrentState;
        pCurrentState = pCurrentState->pParent;
    }
    
    ls32v_NextStateCnt = 0;
    while(pNextState->pEntry)
    {
        pNextState = pNextState->pEntry;
    }
    
    while(pNextState && (ls32v_NextStateCnt<10))
    {
        lptrv_NextStatePath[ls32v_NextStateCnt++] = pNextState;
        pNextState = pNextState->pParent;
    }
    
    for(;(ls32v_CurrentStateCnt>0)&&(ls32v_NextStateCnt>0); (ls32v_CurrentStateCnt--)&&(ls32v_NextStateCnt--))
    {
        if(lptrv_CurrentStatePath[ls32v_CurrentStateCnt-1] != lptrv_NextStatePath[ls32v_NextStateCnt-1])
        {
            break;
        }
    }
    
    /* */
    memset(lptrv_Buf, 0, sizeof(lptrv_Buf));
    for(int32_t ls32v_Cnt=0; ls32v_Cnt<ls32v_CurrentStateCnt; ls32v_Cnt++)
    {
        lptrv_Buf[ls32v_Cnt] = lptrv_CurrentStatePath[ls32v_Cnt];
    }
    
    for(int32_t ls32v_Cnt=0; ls32v_Cnt<ls32v_NextStateCnt; ls32v_Cnt++)
    {
        lptrv_Buf[ls32v_Cnt+10] = lptrv_NextStatePath[ls32v_NextStateCnt-1-ls32v_Cnt];
    }
    
    return (TruState **)&lptrv_Buf;
}

int32_t smHandleEvent(TruFSM *pFSM, TruEvent *pEvent)
{
    int32_t ls32v_Ret = SM_STATE_NO_STATE_CHANGED;
    
    assert(pFSM);
    assert(pEvent);
    
    if(pFSM->pCState == NULL)
    {
        goToErrorState(pFSM, pEvent);
        return SM_ERROR_STATE_REACHED;
    }
    
    if(pFSM->pCState->nNumOfTrans == 0)
    {
        return SM_STATE_NO_STATE_CHANGED;
    }
    
    TruState *lptrv_CurrentState = pFSM->pCState;

    while(lptrv_CurrentState)
    {
        TruTransition *lptrv_Transition = getTransition(pFSM, lptrv_CurrentState, pEvent);
        if(lptrv_Transition != NULL)
        {
            TruState *lptrv_NextState = lptrv_Transition->pNextState;
            while((lptrv_NextState!=NULL)&&(lptrv_NextState->pEntry!=NULL))
            {
                lptrv_NextState = lptrv_NextState->pEntry;
            }
            
            if((lptrv_NextState!=NULL) && (lptrv_NextState!=pFSM->pCState))
            {
                TruState **lptrv_TransitionPath = stateM_getTrasitionPath(pFSM->pCState, lptrv_NextState);
                
                assert(lptrv_TransitionPath);
                
                for(int32_t ls32v_Cnt=0; (lptrv_TransitionPath[ls32v_Cnt]!=NULL); ls32v_Cnt++)
                {
                    TruState *lptrv_State = lptrv_TransitionPath[ls32v_Cnt];
                    
                    lptrv_State->pExitAction(lptrv_State->pData, pEvent);
                }
                
                if(lptrv_Transition->pAction)
                {
                    lptrv_Transition->pAction(lptrv_CurrentState->pData, pEvent, lptrv_Transition->pNextState->pData);
                }
                
                for(int32_t ls32v_Cnt=0; lptrv_TransitionPath[ls32v_Cnt+10]!=NULL; ls32v_Cnt++)
                {
                    TruState *lptrv_State = lptrv_TransitionPath[ls32v_Cnt+10];
                    
                    lptrv_State->pEntryAction(lptrv_State->pData, pEvent);
                }

                pFSM->pPState = pFSM->pCState;
                pFSM->pCState = lptrv_NextState;
                
                if(pFSM->pCState->nNumOfTrans == 0)
                {
                    ls32v_Ret = SM_STATE_FINAL_STATE_REACHED;
                }
                else
                {
                    ls32v_Ret = SM_STATE_CHANGED;
                }
            }
            else if(lptrv_Transition->pAction)
            {
                lptrv_Transition->pAction(lptrv_CurrentState->pData, pEvent, NULL);
            }

            break;
        }
        
        lptrv_CurrentState = lptrv_CurrentState->pParent;
    }   
    
    return ls32v_Ret;
}

TruState *smGetCState( TruFSM *fsm )
{
   if ( !fsm )
      return NULL;

   return fsm->pCState;
}

TruState *smGetPState( TruFSM *fsm )
{
   if ( !fsm )
      return NULL;

   return fsm->pPState;
}


static void goToErrorState( TruFSM *fsm,
      TruEvent *const event )
{
   fsm->pPState = fsm->pCState;
   fsm->pCState = fsm->pEState;

   if ( fsm->pCState && fsm->pCState->pEntryAction )
      fsm->pCState->pEntryAction( fsm->pCState->pData, event );
}

static TruTransition *getTransition( TruFSM *fsm,
      TruState *state, TruEvent *const event )
{
   size_t i;

   for ( i = 0; i < state->nNumOfTrans; ++i )
   {
      TruTransition *t = &state->pTransitions[ i ];

      /* A transition for the given event has been found: */
      if ( t->nEType == event->nType )
      {
         if ( !t->pGuard )
            return t;
         /* If transition is guarded, ensure that the condition is held: */
         else if ( t->pGuard( t->pCond, event ) )
            return t;
      }
   }

   /* No transitions found for given event for given state: */
   return NULL;
}

bool smStop( TruFSM *fsm )
{
   if ( !fsm )
      return true;

   return fsm->pCState->nNumOfTrans == 0;
}
