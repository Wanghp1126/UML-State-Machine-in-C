/**
 * \file
 * \brief   Hierarchical state machine example

 * \author  Nandkishor Biradar
 * \date    01 February 2020

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
*  --------------------- Global variables ---------------------
*/

//! Instance of demo_state_machine_t
static demo_state_machine_t DemoStateMachine;

//! Create and initialize the array of state machines.
static struct StateMachine_t* const State_Machines[] = { (struct StateMachine_t*)&DemoStateMachine };



int main(void)
{
	int i = 0;
	char input[]={'1','1','1','2','1','3'};
	
	// Initialize the demo state machine.
	init_demo(&DemoStateMachine);
	
	while (1)
	{
		for(i=0; i<6; i++)
		{
			parse_cli(&DemoStateMachine, input[i]); 
			if (DispatchEvent(State_Machines, 1) == EVENT_UN_HANDLED)
			{
				printf("invalid event entered\n");
			}
		}
	}
	//return 0;
}
