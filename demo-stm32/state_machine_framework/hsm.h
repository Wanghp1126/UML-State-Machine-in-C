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
#define  HIERARCHICAL_STATES    ( !0 )
#endif // HIERARCHICAL_STATES

//! The logging of state machine
// 0: disable the state machine logger
// !0: enable the state machine logger
#ifndef STATE_MACHINE_LOGGER
#define STATE_MACHINE_LOGGER     ( !0 )      
#endif // STATE_MACHINE_LOGGER

// 0: disable variable length array used in hsm.c
// !0: enable variable length aray used in hsm.c
#ifndef HSM_USE_VARIABLE_LENGTH_ARRAY
#define HSM_USE_VARIABLE_LENGTH_ARRAY  ( !0 )
#endif

/*
 *  --------------------- ENUMERATION ---------------------
 */

 //! List of state machine result code
enum StateMachineResultEnum
{
	EVENT_HANDLED,     //!< 事件处理成功
	EVENT_UN_HANDLED,  //!< 事件未能处理
	TRIGGERED_TO_SELF, //!< 事件处理成功并自我触发
};
/*
 *  --------------------- STRUCTURE ---------------------
 */

struct StateMachine_t;
typedef enum StateMachineResultEnum (*StateHandler)(struct StateMachine_t* const sm);
typedef void (*StateMachineEventLoggerHandler)(uint32_t state_machine, uint32_t state, uint32_t event);
typedef void (*StateMachineResultLoggerHandler)(uint32_t state, enum StateMachineResultEnum result);

#if ( HIERARCHICAL_STATES != 0 )
//! Hierarchical state structure 分层状态结构 
struct StateMachineState_t {
	StateHandler entry;      //!< 此状态的 Entry 动作（Entry Action）
	StateHandler handler;    //!< 此状态的 Do 动作（Do Action）
	StateHandler exit;       //!< 此状态的 Exit 动作（Exit Action）
	
	uint32_t id;          //!< 状态机内的状态的唯一标识

	const struct StateMachineState_t* const parent;    //!< 当前状态的父级状态
	const struct StateMachineState_t* const child;     //!< 当前状态的子级状态
	uint32_t level;                 //!< 从最高状态开始的层次结构级别，数值越小级别越高
};
#else
//! finite state structure 有限状态结构
struct StateMachineState_t {
	StateHandler entry;      //!< 此状态的 Entry 动作（Entry Action）
	StateHandler handler;    //!< 此状态的 Do 动作（Do Action）
	StateHandler exit;       //!< 此状态的 Exit 动作（Exit Action）
	
	uint32_t id;          //!< 状态机内的状态的唯一标识
};
#endif

//! Abstract state machine structure
struct StateMachine_t
{
   uint32_t event;          //!< 挂起的事件
   const struct StateMachineState_t* state; //!< 状态机中的状态
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
#if ( STATE_MACHINE_LOGGER != 0 )
		, StateMachineEventLoggerHandler event_logger
		, StateMachineResultLoggerHandler result_logger
#endif // STATE_MACHINE_LOGGER
		);

#if ( HIERARCHICAL_STATES != 0 )
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
