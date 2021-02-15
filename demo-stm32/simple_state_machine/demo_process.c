/**
 * \file
 * \brief Simple finite state machine example

 * \author  Nandkishor Biradar
 * \date    14 December 2018

 *  Copyright (c) 2018-2019 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

 /*
  *  --------------------- INCLUDE FILES ---------------------
  */
#include <stdint.h>
#include <stdio.h>

#include "hsm.h"
#include "demo_process.h"

  /*
   *  --------------------- ENUMERATION ---------------------
   */

 //! List of states the process state machine
typedef enum
{
	IDLE_STATE,
	ACTIVE_STATE,
	PAUSE_STATE
}ProcessStateIdEnum;

/*
 *  --------------------- Function prototype ---------------------
 */

static enum StateMachineResultEnum idle_handler(struct StateMachine_t* const state);
static enum StateMachineResultEnum idle_entry_handler(struct StateMachine_t* const state);
static enum StateMachineResultEnum idle_exit_handler(struct StateMachine_t* const state);

static enum StateMachineResultEnum active_handler(struct StateMachine_t* const state);
static enum StateMachineResultEnum active_entry_handler(struct StateMachine_t* const state);
static enum StateMachineResultEnum active_exit_handler(struct StateMachine_t* const state);

static enum StateMachineResultEnum paused_handler(struct StateMachine_t* const state);
static enum StateMachineResultEnum paused_entry_handler(struct StateMachine_t* const state);
static enum StateMachineResultEnum paused_exit_handler(struct StateMachine_t* const state);

/*
 *  --------------------- Global variables ---------------------
 */

static const struct StateMachineState_t Process_States[] =
{
  [IDLE_STATE] = {
	.handler = idle_handler,
	.entry = idle_entry_handler,
	.exit = idle_exit_handler,
	.id = IDLE_STATE,
  },

  [ACTIVE_STATE] = {
	.handler = active_handler,
	.entry = active_entry_handler,
	.exit = active_exit_handler,
	.id = ACTIVE_STATE,
  },

  [PAUSE_STATE] = {
	.handler = paused_handler,
	.entry = paused_entry_handler,
	.exit = paused_exit_handler,
	.id = PAUSE_STATE,
  }
};

/*
 *  --------------------- Functions ---------------------
 */

void init_process(process_t* const pProcess, uint32_t processTime)
{
	pProcess->Machine.state = &Process_States[IDLE_STATE];
	pProcess->Machine.event = 0;
	pProcess->Set_Time = processTime;
	pProcess->Resume_Time = 0;

	idle_entry_handler((struct StateMachine_t*)pProcess);
}




static enum StateMachineResultEnum idle_entry_handler(struct StateMachine_t* const pState)
{
	process_t* const pProcess = (process_t*)pState;
	pProcess->Timer = 0;  // Stop process timer

	printf("Entering to idle state\n");
	printf("Supported events\n");
	printf("'s' : Start process\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum idle_handler(struct StateMachine_t* const pState)
{
	switch (pState->event)
	{
	case START:
		return SwitchState(pState, &Process_States[ACTIVE_STATE]);

	default:
		return EVENT_UN_HANDLED;
	}
	//return EVENT_HANDLED;
}

static enum StateMachineResultEnum idle_exit_handler(struct StateMachine_t* const pState)
{
	process_t* const pProcess = (process_t*)pState;
	pProcess->Timer = pProcess->Set_Time;
	printf("Exiting from idle state\n");
	return EVENT_HANDLED;
}




static enum StateMachineResultEnum active_entry_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("Entering to active state\n");
	printf("Supported events\n");
	printf("'q' : stop process\n");
	printf("'p' : Pause process\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum active_handler(struct StateMachine_t* const pState)
{
	switch (pState->event)
	{
	case STOP:
		return SwitchState(pState, &Process_States[IDLE_STATE]);

	case PAUSE:
		return SwitchState(pState, &Process_States[PAUSE_STATE]);

	case TIMEOUT:
		return SwitchState(pState, &Process_States[IDLE_STATE]);

	default:
		return EVENT_UN_HANDLED;
	}
	//return EVENT_HANDLED;
}

static enum StateMachineResultEnum active_exit_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("Exiting from Active state\n");
	return EVENT_HANDLED;
}





static enum StateMachineResultEnum paused_entry_handler(struct StateMachine_t* const pState)
{
	process_t* const pProcess = (process_t*)pState;
	pProcess->Resume_Time = pProcess->Timer;  // Save remaining time
	pProcess->Timer = 0;    // Stop the process timer

	printf("Entering to pause state\n");
	printf("Supported events\n");
	printf("'q' : stop process\n");
	printf("'r' : resume process\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum paused_handler(struct StateMachine_t* const pState)
{
	process_t* const pProcess = (process_t*)pState;
	switch (pState->event)
	{
	case STOP:
		return SwitchState(pState, &Process_States[IDLE_STATE]);

	case RESUME:
		pProcess->Timer = pProcess->Resume_Time;
		return SwitchState(pState, &Process_States[ACTIVE_STATE]);

	default:
		return EVENT_UN_HANDLED;
	}
	//return EVENT_HANDLED;
}

static enum StateMachineResultEnum paused_exit_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("Exiting from paused state\n");
	return EVENT_HANDLED;
}
