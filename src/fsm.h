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

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stddef.h>
#include <stdbool.h>

struct event
{
   int type;
 
   void *data;
};

struct state;


struct transition
{
   int eventType;

   void *condition;
   
   bool ( *guard )( void *condition, struct event *event );

   void ( *action )( void *currentStateData, struct event *event,
         void *newStateData );

   struct state *nextState;
};

struct state
{
   struct state *parentState;

   struct state *entryState;

   struct transition *transitions;

   size_t numTransitions;

   void *data;

   void ( *entryAction )( void *stateData, struct event *event );

   void ( *exitAction )( void *stateData, struct event *event );
};


struct stateMachine
{

   struct state *currentState;

   struct state *previousState;

   struct state *errorState;
};


void stateM_init( struct stateMachine *stateMachine,
      struct state *initialState, struct state *errorState );

enum stateM_handleEventRetVals
{
 
   stateM_errArg = -2,

   stateM_errorStateReached,
   /** \brief The current state changed into a non-final state */
   stateM_stateChanged,

   stateM_stateLoopSelf,

   stateM_noStateChange,
   /** \brief A final state (any but the error state) was reached */
   stateM_finalStateReached,
};


int stateM_handleEvent( struct stateMachine *stateMachine,
      struct event *event );


struct state *stateM_currentState( struct stateMachine *stateMachine );

struct state *stateM_previousState( struct stateMachine *stateMachine );

bool stateM_stopped( struct stateMachine *stateMachine );

#endif // STATEMACHINE_H
