/**
 * \file
 * \brief   toaster oven - Hierarchical state machine example

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
#include "toaster_oven.h"

  /*
   *  --------------------- Global variables ---------------------
   */

   //! Instance of oven_t
static oven_t SampleOven;

//! Create and initialize the array of state machines.
static struct StateMachine_t* const State_Machines[] = { (struct StateMachine_t*)&SampleOven };


/*
 *  --------------------- Functions ---------------------
 */

// /** \brief Simulate the timer ISR.
//  *
//  * This is an one second timer. When Oven is ON it prints the remaining time on console.
//  * It also generates the timeout event when on time expires.
//  */
//void* timer(void* vargp)
//{
//	(void)(vargp);
//	while (1)
//	{
//		sleep(1);

//		if (SampleOven.Timer > 0)
//		{
//			SampleOven.Timer--;

//			printf("\rRemaining ON time: %d ", SampleOven.Timer);

//			if (SampleOven.Timer == 0)
//			{
//				printf("\n");
//				on_oven_timedout(&SampleOven);  // Generate the timeout event
//				sem_post(&Semaphore);   // signal to main thread
//			}
//		}
//	}
//	return NULL;
//}



int main(void)
{
	int i;
	char input[]={'s','o','c','q'};
		
	// Initialize the oven state machine.
	init_oven(&SampleOven, 10, DOOR_CLOSED);

	while (1)
	{
		for(i=0; i<4; i++)
		{
			parse_cli(&SampleOven, input[i]);

			if (DispatchEvent(State_Machines, 1) == EVENT_UN_HANDLED)
			{
				printf("invalid event entered\n");
			}
		}
	}
//	return 0;
}
