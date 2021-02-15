/**
 * \file
 * \brief Oven hierarchical state machine example

 * \author  Nandkishor Biradar
 * \date    03 February 2020

 *  Copyright (c) 2018-2020 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

 /*
  *  --------------------- INCLUDE FILES ---------------------
  */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "hsm.h"
#include "toaster_oven.h"

/*
*  --------------------- DEFINITION ---------------------
*/


/*
*  --------------------- ENUMERATION ---------------------
*/


//! List of states in oven state machine
typedef enum
{
	DOOR_OPEN_STATE,
	DOOR_CLOSE_STATE
}oven_state_t;

//! List of states in door closed states machine
typedef enum
{
	OFF_STATE,
	ON_STATE,
	TOTAL_DOOR_CLOSED_STATES
}door_close_state_t;

/*
 *  --------------------- Function prototype ---------------------
 */

static enum StateMachineResultEnum door_open_handler(struct StateMachine_t* const pState);
static enum StateMachineResultEnum door_open_entry_handler(struct StateMachine_t* const pState);

static enum StateMachineResultEnum door_close_entry_handler(struct StateMachine_t* const pState);

static enum StateMachineResultEnum off_handler(struct StateMachine_t* const pState);
static enum StateMachineResultEnum off_entry_handler(struct StateMachine_t* const pState);

static enum StateMachineResultEnum on_handler(struct StateMachine_t* const pState);
static enum StateMachineResultEnum on_entry_handler(struct StateMachine_t* const pState);
static enum StateMachineResultEnum on_exit_handler(struct StateMachine_t* const pState);

/*
 *  --------------------- Global variables ---------------------
 */

static const struct StateMachineState_t Door_Close_State[TOTAL_DOOR_CLOSED_STATES];

//! List of all the states
static const struct StateMachineState_t Oven_State[] =
{
	[DOOR_OPEN_STATE] = {
		.handler 	= door_open_handler,
		.entry	 	= door_open_entry_handler,
		.exit 		= NULL,	
	},
	
	[DOOR_CLOSE_STATE] = {  
		.handler 	= NULL,
		.entry	 	= door_close_entry_handler,	
		.exit 		= NULL,	
		.level		= 0,
		.child		= Door_Close_State,
	},
};

static const struct StateMachineState_t Door_Close_State[TOTAL_DOOR_CLOSED_STATES] =
{
	[OFF_STATE] = {
		.handler 	= off_handler,
		.entry	 	= off_entry_handler,
		.exit 		= NULL,
		.level		= 1,
		.parent   = &Oven_State[DOOR_CLOSE_STATE], 
	},
	
	[ON_STATE] = {  
		.handler 	= on_handler,
		.entry	 	= on_entry_handler,	
		.exit 		= on_exit_handler,
		.level		= 1,
		.parent   = &Oven_State[DOOR_CLOSE_STATE], 
	},
};


/*
 *  --------------------- Functions ---------------------
 */

void init_oven(oven_t* const pOven, uint32_t toastTime, door_status_t status)
{
	if (status == DOOR_CLOSED)
	{
		pOven->Machine.state = &Door_Close_State[OFF_STATE];
	}
	else
	{
		pOven->Machine.state = &Oven_State[DOOR_OPEN_STATE];
	}
	pOven->Machine.event = 0;
	pOven->Set_Time = toastTime;
	pOven->Resume_Time = 0;
	pOven->Timer = 0;
	pOven->Lamp = false;
	pOven->Heater = false;

	printf("Oven is initialized\n");
	printf("Door is closed and oven is off\n");
	printf("Press 's': to turn on Oven\n");
	printf("Press 'o': to open the door\n");
}



static enum StateMachineResultEnum door_open_entry_handler(struct StateMachine_t* const pState)
{
	printf("Turn on Oven lamp\n");
	((oven_t*)pState)->Lamp = true;

	printf("Press 'c': to close the door\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum door_open_handler(struct StateMachine_t* const pState)
{
	oven_t* const pOven = (oven_t*)pState;
	switch (pState->event)
	{
	case EN_DOOR_CLOSE:
		if (pOven->Resume_Time)
		{
			return TraverseState(pState, &Door_Close_State[ON_STATE]);
		}
		else
		{
			return TraverseState(pState, &Door_Close_State[OFF_STATE]);
		}

	default:
		return EVENT_UN_HANDLED;
	}
}



static enum StateMachineResultEnum door_close_entry_handler(struct StateMachine_t* const pState)
{
	printf("Turn off Oven lamp\n");
	((oven_t*)pState)->Lamp = false;
	return EVENT_HANDLED;
}


static enum StateMachineResultEnum off_entry_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("Press 's': to turn on Oven\n");
	printf("Press 'o': to open the door\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum off_handler(struct StateMachine_t* const pState)
{
	oven_t* const pOven = (oven_t*)pState;
	switch (pState->event)
	{
	case EN_START:
		pOven->Timer = pOven->Set_Time;
		pOven->Resume_Time = 0;
		// Use SwitchState when target state is within the same state machine.
		// i.e. target and source states have same parent state.
		return SwitchState(pState, &Door_Close_State[ON_STATE]);

	case EN_DOOR_OPEN:
		// Use TraverseState when target and source states doesn't have same parent state.
		return TraverseState(pState, &Oven_State[DOOR_OPEN_STATE]);

	default:
		return EVENT_UN_HANDLED;
	}
}



static enum StateMachineResultEnum on_entry_handler(struct StateMachine_t* const pState)
{
	oven_t* const pOven = (oven_t*)pState;
	printf("Turn on heater\n");
	pOven->Heater = true;

	if (pOven->Resume_Time)
	{
		pOven->Timer = pOven->Resume_Time;
		pOven->Resume_Time = 0;
	}
	printf("Press 'q': to turn off Oven\n");
	printf("Press 'o': to open the door\n");

	return EVENT_HANDLED;
}

static enum StateMachineResultEnum on_handler(struct StateMachine_t* const pState)
{
	oven_t* const pOven = (oven_t*)pState;
	switch (pState->event)
	{
	case EN_STOP:
		pOven->Timer = 0;
		pOven->Resume_Time = 0;
		return SwitchState(pState, &Door_Close_State[OFF_STATE]);

	case EN_TIMEOUT:
		return SwitchState(pState, &Door_Close_State[OFF_STATE]);

	case EN_DOOR_OPEN:
		pOven->Resume_Time = pOven->Timer;
		pOven->Timer = 0;
		return TraverseState(pState, &Oven_State[DOOR_OPENED]);

	default:
		return EVENT_UN_HANDLED;
	}
}

static enum StateMachineResultEnum on_exit_handler(struct StateMachine_t* const pState)
{
	oven_t* const pOven = (oven_t*)pState;
	printf("Turn Off heater\n");
	pOven->Heater = false;
	return EVENT_HANDLED;
}
