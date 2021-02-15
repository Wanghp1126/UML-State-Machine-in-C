/**
 * \file
 * \brief hierarchical state machine

 * \author  Nandkishor Biradar
 * \date    17 December 2018

 *  Copyright (c) 2018-2019 Nandkishor Biradar
 *  https://github.com/kiishor

 *  Distributed under the MIT License, (See accompanying
 *  file LICENSE or copy at https://mit-license.org/)
 */

#ifndef HSM_CONFIG_H
#define HSM_CONFIG_H

// configuration file for hierarchical state machine.

// Enable the hierarchical state machine
// 0 : disable hierarchical state machine. Only Finite state machine is supported.
// 1 : enable Hierarchical state machine. Both finite and hierarchical state machine is supported.
#define  HIERARCHICAL_STATES      1

// Enable the state machine logging
// 0: disable the state machine logger
// 1: enable the state machine logger
#define STATE_MACHINE_LOGGER     0

#endif // HSM_CONFIG_H
