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

#ifndef HSM_H
#define HSM_H

#ifdef HSM_CONFIG
#include "hsm_config.h"
#endif // HSM_CONFIG

#include "stm32f10x.h"

 /*
  *  --------------------- DEFINITION ---------------------
  */
  
//! finite/hierarchical state machine
// 0 : disable hierarchical state machine. Only Finite state machine is supported.
// !0 : enable Hierarchical state machine. Both finite and hierarchical state machine is supported.
#ifndef HIERARCHICAL_STATES
#define  HIERARCHICAL_STATES    !0
#endif // HIERARCHICAL_STATES

//! The logging of state machine
// 0: disable the state machine logger
// !0: enable the state machine logger
#ifndef STATE_MACHINE_LOGGER
#define STATE_MACHINE_LOGGER     !0        
#endif // STATE_MACHINE_LOGGER

// 0: disable variable length array used in hsm.c
// !0: enable variable length aray used in hsm.c
#ifndef HSM_USE_VARIABLE_LENGTH_ARRAY
#define HSM_USE_VARIABLE_LENGTH_ARRAY  !0
#endif

/*
 *  --------------------- ENUMERATION ---------------------
 */

 //! List of state machine result code
enum StateMachineResultEnum
{
	EVENT_HANDLED,     //!< Event handled successfully.
	EVENT_UN_HANDLED,  //!< Event could not be handled.
	TRIGGERED_TO_SELF, //!< Handler handled the Event successfully, and posted new event to itself.
};

/*
 *  --------------------- STRUCTURE ---------------------
 */

struct StateMachine_t;
typedef enum StateMachineResultEnum (*StateHandler)(struct StateMachine_t* const sm);
typedef void (*StateMachineEventLoggerHandler)(uint32_t state_machine, uint32_t state, uint32_t event);
typedef void (*StateMachineResultLoggerHandler)(uint32_t state, enum StateMachineResultEnum result);

#if HIERARCHICAL_STATES != 0
//! Hierarchical state structure 分层状态结构 
struct StateMachineState_t {
	StateHandler entry;      //!< Entry action in this state
	StateHandler handler;    //!< Do action in this state
	StateHandler exit;       //!< Exit action in this state.
	
	uint32_t id;          //!< unique identifier of a state within the single state machine

	const struct StateMachineState_t* const parent;    //!< Parent state of the current state.
	const struct StateMachineState_t* const child;     //!< Child states of the current state.
	uint32_t level;                 //!< Hierarchy level from the top state.
};
#else
//! finite state structure 有限状态结构
struct StateMachineState_t {
	StateHandler entry;      //!< Entry action in this state
	StateHandler handler;    //!< Do action in this state
	StateHandler exit;       //!< Exit action in this state.
	
	uint32_t id;          //!< unique identifier of a state within the single state machine
};
#endif

//! Abstract state machine structure
struct StateMachine_t
{
	uint32_t event;              //!< Pending Event for state machine, cannot be 0.
	const struct StateMachineState_t* state; //!< State of state machine.
};


/*
 *  --------------------- EXPORTED FUNCTION ---------------------
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	extern enum StateMachineResultEnum 
		DispatchEvent(struct StateMachine_t* const pStateMachine[],
		uint32_t quantity
#if STATE_MACHINE_LOGGER
		, StateMachineEventLoggerHandler event_logger
		, StateMachineResultLoggerHandler result_logger
#endif // STATE_MACHINE_LOGGER
		);

#if HIERARCHICAL_STATES
	extern enum StateMachineResultEnum 
		TraverseState(struct StateMachine_t* const pStateMachine,
		const struct StateMachineState_t* pTargetState);
#endif // HIERARCHICAL_STATES

	extern enum StateMachineResultEnum 
		SwitchState(struct StateMachine_t* const pStateMachine,
		const struct StateMachineState_t* const pTargetState);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HSM_H
