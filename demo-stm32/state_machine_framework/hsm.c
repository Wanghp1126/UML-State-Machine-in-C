/**
 * \file
 * \brief hierarchical state machine

 * \author  Nandkishor Biradar
 * \date    01 December 2018

 *  Copyright (c) 2018-2019 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

/*
 *  --------------------- INCLUDE FILES ---------------------
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "hsm.h"

/*
 *  --------------------- DEFINITION ---------------------
 */

#define EXECUTE_HANDLER(handler, triggered, state_machine)      \
do{                                                             \
  if(handler != NULL)                                           \
  {                                                             \
    enum StateMachineResultEnum result;                         \
	result = handler(state_machine);                            \
    switch(result)                                              \
    {                                                           \
    case TRIGGERED_TO_SELF:                                     \
      triggered = true;                                         \
    case EVENT_HANDLED:                                         \
      break;                                                    \
                                                                \
    default:                                                    \
      return result;                                            \
    }                                                           \
  }                                                             \
} while(0)

/*
 *  --------------------- FUNCTION BODY ---------------------
 */

/** \brief dispatch events to state machine
 *
 * \param pStateMachine[] StateMachine_t* const  array of state machines
 * \param quantity uint32_t number of state machines
 * \return StateMachineResultEnum result of state machine
 *
 */
enum StateMachineResultEnum 
	DispatchEvent(struct StateMachine_t* const pStateMachine[]
	, uint32_t quantity
#if ( STATE_MACHINE_LOGGER != 0 )
	, StateMachineEventLoggerHandler event_logger
	, StateMachineResultLoggerHandler result_logger
#endif // STATE_MACHINE_LOGGER
	)
{
	enum StateMachineResultEnum result;
	uint32_t index; // The index in State Machines
	const struct StateMachineState_t* pState;

	// Iterate through all state machines in the array to check if event is pending to dispatch.
	for (index = 0; index < quantity;)
	{
		if (pStateMachine[index]->event == 0)
		{
			index++;
			continue;
		}

		pState = pStateMachine[index]->state;
		do
		{
#if ( STATE_MACHINE_LOGGER != 0 )
			event_logger(index, pState->id, pStateMachine[index]->event);
#endif // STATE_MACHINE_LOGGER
			// Call the state handler.
			result = pState->handler(pStateMachine[index]);
#if ( STATE_MACHINE_LOGGER != 0 )
			result_logger(pStateMachine[index]->state->id, result);
#endif // STATE_MACHINE_LOGGER

			switch (result)
			{
			case EVENT_HANDLED:
				// Clear event, if successfully handled by state handler.
				pStateMachine[index]->event = 0;

				// intentional fall through

				// State handler handled the previous event successfully,
				// and posted a new event to itself.
			case TRIGGERED_TO_SELF:

				index = 0;  // Restart the event dispatcher from the first state machine.
				break;

#if ( HIERARCHICAL_STATES != 0)
				// State handler could not handled the event.
				// Traverse to its parent state and dispatch event to parent state handler.
			case EVENT_UN_HANDLED:

				do
				{
					// check if state has parent state.
					if (pState->parent == NULL)   // Is child reached top
					{
						// This is a fatal error. terminate state machine.
						return EVENT_UN_HANDLED;
					}

					pState = pState->parent;        // traverse to parent state
				} while (pState->handler == NULL);   // repeat again if parent state doesn't have handler
				continue;
#endif // HIERARCHICAL_STATES

				// Either state handler could not handle the event or it has returned
				// the unknown return code. Terminate the state machine.
			default:
				return result;
			}
			break;

		} while (1);
	}
	return EVENT_HANDLED;
}

/** \brief Switch to target states without traversing to hierarchical levels.
 *
 * \param pStateMachine StateMachine_t* const   pointer to state machine
 * \param pTargetState const StateMachineState_t* const     Target state to traverse
 * \return extern StateMachineResultEnum         Result of state traversal
 *
 */
enum StateMachineResultEnum 
	SwitchState(struct StateMachine_t* const pStateMachine,
	const struct StateMachineState_t* const pTargetState)
{
	const struct StateMachineState_t* const pSource_State = pStateMachine->state;
	bool triggered_to_self = false;
	pStateMachine->state = pTargetState;    // Save the target child

	// Call Exit function before leaving the Source State.
	EXECUTE_HANDLER(pSource_State->exit, triggered_to_self, pStateMachine);

	// Call Entry function before entering the Target State.
	EXECUTE_HANDLER(pTargetState->entry, triggered_to_self, pStateMachine);

	if (triggered_to_self == true)
	{
		return TRIGGERED_TO_SELF;
	}

	return EVENT_HANDLED;
}

#if ( HIERARCHICAL_STATES != 0 )
/** \brief Traverse to target state. It calls exit functions before leaving
	  the source state & calls entry function before entering the target state.
 *
 * \param pStateMachine StateMachine_t* const   pointer to state machine
 * \param pTargetState const StateMachineState_t*           Target state to traverse
 * \return StateMachineResultEnum                Result of state traversal
 *
 */
enum StateMachineResultEnum 
	TraverseState(struct StateMachine_t* const pStateMachine,
	const struct StateMachineState_t* pTargetState)
{
	const struct StateMachineState_t* pSource_State = pStateMachine->state;
	bool triggered_to_self = false;
	pStateMachine->state = pTargetState;    // Save the target child

#if (HSM_USE_VARIABLE_LENGTH_ARRAY == 1)
	const struct StateMachineState_t* pTargetPath[pTargetState->level];  // Array to store the target child path
#else
#if  (!defined(MAX_HIERARCHICAL_LEVEL) || (MAX_HIERARCHICAL_LEVEL == 0))
#error "MAX_HIERARCHICAL_LEVEL is undefined."\
         "Define the maximum hierarchical level of the state machine or \
          use variable length array by setting HSM_USE_VARIABLE_LENGTH_ARRAY to 1"
#endif

	const StateMachineState_t* pTargetPath[MAX_HIERARCHICAL_LEVEL];     // Array to store the target child path
#endif

	uint32_t index = 0;

	// make the source state & target state at the same hierarchy level.

	// Is source hierarchy level is less than target hierarchy level?
	if (pSource_State->level > pTargetState->level)
	{
		// Traverse the source state to upward,
		// till it matches with target state hierarchy level.
		while (pSource_State->level > pTargetState->level)
		{
			EXECUTE_HANDLER(pSource_State->exit, triggered_to_self, pStateMachine);
			pSource_State = pSource_State->parent;
		}
	}
	// Is Source hierarchy level greater than target level?
	else if (pSource_State->level < pTargetState->level)
	{
		// Traverse the target state to upward,
		// Till it matches with source state hierarchy level.
		while (pSource_State->level < pTargetState->level)
		{
			pTargetPath[index++] = pTargetState;  // Store the target child path.
			pTargetState = pTargetState->parent;
		}
	}

	// Now Source & Target are at same hierarchy level.
	// Traverse the source & target state to upward, till we find their common parent.
	while (pSource_State->parent != pTargetState->parent)
	{
		EXECUTE_HANDLER(pSource_State->exit, triggered_to_self, pStateMachine);
		pSource_State = pSource_State->parent;  // Move source state to upward state.

		pTargetPath[index++] = pTargetState;    // Store the target child path.
		pTargetState = pTargetState->parent;    // Move the target state to upward state.
	}

	// Call Exit function before leaving the Source state.
	EXECUTE_HANDLER(pSource_State->exit, triggered_to_self, pStateMachine);
	// Call entry function before entering the target state.
	EXECUTE_HANDLER(pTargetState->entry, triggered_to_self, pStateMachine);

	// Now traverse down to the target child & call their entry functions.
	while (index)
	{
		index--;
		EXECUTE_HANDLER(pTargetPath[index]->entry, triggered_to_self, pStateMachine);
	}

	if (triggered_to_self == true)
	{
		return TRIGGERED_TO_SELF;
	}
	return EVENT_HANDLED;
}
#endif // HIERARCHICAL_STATES

