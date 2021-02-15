/**
 * \file
 * \brief   Simple state machine example

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
   *  --------------------- Global variables ---------------------
   */

//! Instance of process_t
static process_t SampleProcess;

//! Create and initialize the array of state machines.
static struct StateMachine_t* const State_Machines[] = { (struct StateMachine_t*)&SampleProcess };


/*
 *  --------------------- Functions ---------------------
 */

 //! Callback function to log the events dispatched by state machine framework.
static void event_logger(uint32_t stateMachine, uint32_t state, uint32_t event)
{
	printf("state Machine: %d, state: %d, event: %d\n", stateMachine, state, event);
}

//! Callback function to log the result of event processed by state machine
static void result_logger(uint32_t state, enum StateMachineResultEnum result)
{
	printf("Result: %d, New state: %d\n", result, state);
}



int main(void)
{
	int i = 0;
	char input[]={'s','p','q'};
	
	// Initialize the process state machine.
	init_process(&SampleProcess, 10);

	while (1)
	{
		for(i=0; i<3; i++)
		{
			parse_cli(&SampleProcess, input[i]); 

			if (DispatchEvent(State_Machines, 1, event_logger, result_logger) == EVENT_UN_HANDLED)
			{
				printf("invalid event entered\n");
			}
		}
	}
	//return 0;
}

