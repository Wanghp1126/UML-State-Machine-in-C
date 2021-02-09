/**
 * \file
 * \brief hierarchical state machine demo

 * \author  Nandkishor Biradar
 * \date    12 February 2020

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
#include "demo.h"

  /*
   *  --------------------- Function prototype ---------------------
   */

static enum StateMachineResultEnum level1_state1_handler(struct StateMachine_t* const);
static enum StateMachineResultEnum level1_state1_entry_handler(struct StateMachine_t* const);
static enum StateMachineResultEnum level1_state1_exit_handler(struct StateMachine_t* const);

static enum StateMachineResultEnum level1_state2_handler(struct StateMachine_t* const);
static enum StateMachineResultEnum level1_state2_entry_handler(struct StateMachine_t* const);
static enum StateMachineResultEnum level1_state2_exit_handler(struct StateMachine_t* const);

static enum StateMachineResultEnum level2_state1_handler(struct StateMachine_t* const);
static enum StateMachineResultEnum level2_state1_entry_handler(struct StateMachine_t* const);
static enum StateMachineResultEnum level2_state1_exit_handler(struct StateMachine_t* const);

static enum StateMachineResultEnum level3_state1_handler(struct StateMachine_t* const);
static enum StateMachineResultEnum level3_state1_entry_handler(struct StateMachine_t* const);
static enum StateMachineResultEnum level3_state1_exit_handler(struct StateMachine_t* const);

/*
 *  --------------------- Global variables ---------------------
 */

static const struct StateMachineState_t Level2_HSM[1];
static const struct StateMachineState_t Level3_HSM[1];

// This is an array of root (top most) states .
static const struct StateMachineState_t Level1_HSM[] =
{
  {
	.handler = level1_state1_handler,       // state handler
	.entry = level1_state1_entry_handler,   // Entry action handler
	.exit = level1_state1_exit_handler,     // Exit action handler
	.parent = NULL,                         // Parent state
	.child = NULL,                          // Child states
	.level = 0                              // Hierarchical state level
  },
  {
	.handler = level1_state2_handler,       // state handler
	.entry = level1_state2_entry_handler,   // Entry action handler
	.exit = level1_state2_exit_handler,     // Exit action handler
	.parent = NULL,                         // Parent state
	.child = Level2_HSM,                    // Child state
	.level = 0                              // Hierarchical state level
  }
};

// This is an array of substates of Level1_HSM[1] composite state.
static const struct StateMachineState_t Level2_HSM[] =
{
  {
	.handler = level2_state1_handler,       // state handler
	.entry = level2_state1_entry_handler,   // Entry action handler
	.exit = level2_state1_exit_handler,     // Exit action handler
	.parent = &Level1_HSM[1],               // Parent state
	.child = &Level3_HSM[0],                // Child state
	.level = 1                              // Hierarchical state level
  }
};

// This is an array of substates of Level2_HSM[0] composite state.
static const struct StateMachineState_t Level3_HSM[] =
{
  {
	.handler = level3_state1_handler,       // state handler
	.entry = level3_state1_entry_handler,   // Entry action handler
	.exit = level3_state1_exit_handler,     // Exit action handler
	.parent = &Level2_HSM[0],               // Parent state
	.child = NULL,                          // Child state
	.level = 2                              // Hierarchical state level
  }
};

/*
 *  --------------------- Functions ---------------------
 */

void init_demo(demo_state_machine_t* const pDemo)
{
	pDemo->Machine.state = &Level1_HSM[0];
	pDemo->Machine.event = 0;
	printf("State machine is in L1_S1 state\n");
	printf("Supported Events are\n");
	printf("EVENT1: press 1 to trigger\n");
}

static enum StateMachineResultEnum level1_state1_handler(struct StateMachine_t* const pState)
{
	switch (pState->event)
	{
	case EVENT1:
		printf("L1_S1: event-1 is processed successfully\n");
		return TraverseState(pState, &Level3_HSM[0]);

	default:
		printf("L1_S1: handler does not handle this event\n");
		return EVENT_UN_HANDLED;
	}
}

static enum StateMachineResultEnum level1_state1_entry_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("L1_S1: entry handler called\n");
	printf("\nState machine is in L1_S1 state\n");
	printf("Supported Events are\n");
	printf("EVENT1: press 1 to trigger\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum level1_state1_exit_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("L1_S1: exit handler called\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum level1_state2_handler(struct StateMachine_t* const pState)
{
	switch (pState->event)
	{
	case EVENT3:
		printf("L1_S2: event-3 is processed successfully\n");
		return TraverseState(pState, &Level1_HSM[0]);

	default:
		printf("L1_S2: handler does not handle this event\n");
		return EVENT_UN_HANDLED;
	}
}

static enum StateMachineResultEnum level1_state2_entry_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("L1_S2: entry handler called\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum level1_state2_exit_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("L1_S2: exit handler called\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum level2_state1_handler(struct StateMachine_t* const pState)
{
	switch (pState->event)
	{
	case EVENT2:
		printf("L2_S1: event-2 is processed successfully\n");
		return TraverseState(pState, &Level1_HSM[0]);

	default:
		printf("L2_S1: handler does not handle this event\n");
		return EVENT_UN_HANDLED;
	}
	//return EVENT_HANDLED;
}

static enum StateMachineResultEnum level2_state1_entry_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("L2_S1: entry handler called\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum level2_state1_exit_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("L2_S1: exit handler called\n");
	return EVENT_HANDLED;
}

static enum StateMachineResultEnum level3_state1_handler(struct StateMachine_t* const pState)
{
	switch (pState->event)
	{
	case EVENT1:
		printf("L3_S1: event-1 is processed successfully\n");
		return TraverseState(pState, &Level1_HSM[0]);

	default:
		printf("L3_S1: handler does not handle this event\n");
		return EVENT_UN_HANDLED;
	}
	//return EVENT_HANDLED;
}

static enum StateMachineResultEnum level3_state1_entry_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("L3_S1: entry handler called\n");

	printf("\nState machine is in L3_S1 state\n");
	printf("Supported Events are\n");
	printf("EVENT1: press 1 to trigger\n");
	printf("EVENT2: press 2 to trigger\n");
	printf("EVENT3: press 3 to trigger\n");

	return EVENT_HANDLED;
}

static enum StateMachineResultEnum level3_state1_exit_handler(struct StateMachine_t* const pState)
{
	(void)(pState);
	printf("L3_S1: exit handler called\n");
	return EVENT_HANDLED;
}
