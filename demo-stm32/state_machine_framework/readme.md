# 前言

- 本文件中的内容参考：https://github.com/kiishor/UML-State-Machine-in-C。

- 对原文和代码中的API、结构体、枚举、部分变量的名称就行了修改。

- 禁止对 struct 使用 typedef。

- 在MDK中使用STM32F103ZE对原作者的 Demo 做了软件仿真，工程链接为：https://github.com/Wanghp1126/UML-State-Machine-in-C/tree/Wanghp1126-STM32-MDK-Demo/demo-stm32。

- 使用此框架，应尊重原作者的知识产权。


# 1 框架简介

这是用C实现的UML状态机的轻量级框架。它支持有限状态机和分层状态机。该框架独立于CPU和操作系统，并且专门为嵌入式应用程序而开发。
- 只有3个API
	1. `DispatchEvent`：发生事件时，调用此API，将事件调度到状态机。
	2. `SwitchState`：在有限状态机中，满足状态转移时，调用此API，进行状态的切换；一般在本状态的`hander`中调用。
	3. `TraverseState`：在分层状态机中，满足状态转移时，调用此API，进行状态的切换；一般在本状态的`hander`中调用。
- 有2个结构体
	1. `StateMachineState_t`：单个状态的结构体，包含本状态的id、进入状态的动作、退出状态的动作、状态内的动作
	2. `StateMachine_t`：状态机的结构体，包含事件和状态
- 有1个枚举
	`StateMachineResultEnum`：事件处理结果的枚举。
- 框架包含三个文件
	1. hsm.c：框架的实现。
	2. hsm.h：包含API和结构声明。
	3. hsm_config.h：框架的编译时配置，框架的功能应该在此文件中修改，而不应该修改hsm.h文件中的宏定义。

更多关于有限状态机和分层状态机，可以参考如下连接：
<https://en.wikipedia.org/wiki/UML_state_machine> 
<https://en.wikipedia.org/wiki/Finite-state_machine>

# 2 详细介绍
## 2.1 状态机结构体
-------------
状态机由`StateMachine_t`数据类型表示。它是可以继承以创建状态机的抽象结构体。`StateMachine_t`必须在衍生状态机的第一个成员，以方便使用时强制类型转换。
```C
//! 抽象的状态机结构体
struct StateMachine_t
{
   uint32_t event;          //!< 挂起的事件
   const struct StateMachineState_t* state; //!< 状态机中的状态
};
```
示例：
```C
//! 派生的状态机
struct UserStateMachine
{
  struct StateMachine_t machine;    //!< 必须是第一个成员

  // 用户特定的成员
  uint32_t param1;
  uint32_t param2;
  uint32_t param3;
};
```
## 2.2 事件
事件由32位的无符号数表示，`StateMachine_t`中的`event`存储传送给状态机的事件值。`event`等于0表示状态机已准备好接受新的事件；写入任何非零值，将事件传递给状态机；当状态机成功处理事件后，框架将`event`清零。当`event`不为零时，请勿写入新的事件值。在这种情况下，当状态机仍在忙于处理事件时，建议使用队列来推送新的事件值。
## 2.3 事件处理的结果
事件处理由3种结果：事件处理成功、事件未能处理、事件处理成功并自我触发
```C
enum StateMachineResultEnum
{
	EVENT_HANDLED,     //!< 事件处理成功
	EVENT_UN_HANDLED,  //!< 事件未能处理
	TRIGGERED_TO_SELF, //!< 事件处理成功并自我触发
};
```
## 2.4 状态
单个状态由指向`StateMachineState_t`结构体的指针表示。
当框架配置为有限状态机时，则`StateMachineState_t`被定义为：
```C
struct StateMachineState_t {
	StateHandler entry;      //!< 此状态的 Entry 动作（Entry Action）
	StateHandler handler;    //!< 此状态的 Do 动作（Do Action）
	StateHandler exit;       //!< 此状态的 Exit 动作（Exit Action）
	
	uint32_t id;          //!< 状态机内的状态的唯一标识
};
```
当框架配置为分层状态机时，则`StateMachineState_t`被定义为：
```C
struct StateMachineState_t {
	StateHandler entry;      //!< 此状态的 Entry 动作（Entry Action）
	StateHandler handler;    //!< 此状态的 Do 动作（Do Action）
	StateHandler exit;       //!< 此状态的 Exit 动作（Exit Action）
	
	uint32_t id;          //!< 状态机内的状态的唯一标识

	const struct StateMachineState_t* const parent;    //!< 当前状态的父级状态
	const struct StateMachineState_t* const child;     //!< 当前状态的子级状态
	uint32_t level;                 //!< 从最高状态开始的层次结构级别，数值越小级别越高
};
```
Entry\Do\Exit 动作的函数指针：
```C
typedef enum StateMachineResultEnum (*StateHandler)(struct StateMachine_t* const sm);
```
- handler: 指向此状态处理事件的函数指针
- entry: 指向进入此状态时需要处理动作的函数指针
- exit: 指向退出此状态时需要处理动作的函数指针

>handler必须初始化。

>entry和exit时可选项；不需要时可以初始化为null。

>StateMachineState_t 可以定义为 **const** ，以便将其存储在只读存储器中。

有限状态机，使用示例：
```C
enum Status
{
    IDLE_STATE,
    START_STATE,
    STOP_STATE
};

static enum StateMachineResultEnum idle_entry_handler(struct StateMachine_t* const sm);
static enum StateMachineResultEnum idle_do_handler(struct StateMachine_t* const sm);
static enum StateMachineResultEnum idle_exit_handler(struct StateMachine_t* const sm);

static enum StateMachineResultEnum start_entry_handler(struct StateMachine_t* const sm);
static enum StateMachineResultEnum start_do_handler(struct StateMachine_t* const sm);
static enum StateMachineResultEnum start_exit_handler(struct StateMachine_t* const sm);

static enum StateMachineResultEnum stop_entry_handler(struct StateMachine_t* const sm);
static enum StateMachineResultEnum stop_do_handler(struct StateMachine_t* const sm);
static enum StateMachineResultEnum stop_exit_handler(struct StateMachine_t* const sm);

static const struct StateMachineState_t MyStateMachineStatus[] =
{
	[IDLE_STATE] = 	{
		.entry = idle_entry_handler,
		.handler = idle_do_handler,		
		.exit = idle_exit_handler,
		.id = IDLE_STATE,
	},

	[START_STATE] = {
		.entry = start_entry_handler,
		.handler = start_do_handler,		
		.exit = start_exit_handler,
		.id = START_STATE,
	},
	
	[STOP_STATE] = {
		.entry = stop_entry_handler,
		.handler = stop_do_handler,		
		.exit = stop_exit_handler,
		.id = STOP_STATE,
	}
};
```
分层状态机，使用示例：
```C
enum L0Status
{
    L0S0,
    L0S1,
};
enum L1Status
{
    L1S0,
    L1S1
};
enum L2Status
{
    L2S0,
};

static const struct StateMachineState_t L0StateMachineStatus[] =
{
	[L0S0] = {
		.entry = l0s0_entry_handler,
		.handler = l0s0_do_handler,		
		.exit = l0s0_exit_handler,
		.id = L0S0,
		
		.level = 0,
		.parent = null,
		.child = null,
	},

	[L0S1] = {
		.entry = l0s1_entry_handler,
		.handler = l0s1_do_handler,		
		.exit = l0s1_exit_handler,
		.id = L0S1,
		
		.level = 0,
		.parent = null,
		.child = &L1StateMachineStatus[0],
	}
};
static const struct StateMachineState_t L1StateMachineStatus[] =
{
	[L1S0] = {
		.entry = l1s0_entry_handler,
		.handler = l1s0_do_handler,		
		.exit = l1s0_exit_handler,
		.id = L1S0,
		
		.level = 1,
		.parent = null,
		.child = null,
	},

	[L1S1] = {
		.entry = l1s1_entry_handler,
		.handler = l1s1_do_handler,		
		.exit = l1s1_exit_handler,
		.id = L1S1,
		
		.level = 1,
		.parent = &L0StateMachineStatus[0],
		.child = &L2StateMachineStatus[0],
	}
};
static const struct StateMachineState_t L2StateMachineStatus[] =
{
	[L2S0] = {
		.entry = l2s0_entry_handler,
		.handler = l2s0_do_handler,		
		.exit = l2s0_exit_handler,
		.id = L2S0,
		
		.level = 2,
		.parent = &L1StateMachineStatus[0],
		.child = null,
	}
};
```
为了简化代码，可以增加如下宏定义
```C
#define FSM_ADD_STATE(name, entry_handler, do_handler, exit_handler) \
    [name] = {                    \
        .entry   = entry_handler, \
        .handler = do_handler,    \
        .exit = exit_handler,     \
	.id = name,               \
}
#define HSM_ADD_STATE(name, entry_handler, do_handler, exit_handler, level, parent, child) \
    [name] = {                    \
        .entry   = entry_handler, \
        .handler = do_handler,    \
        .exit = exit_handler,     \
        .id = name,               \
        .level = level,           \
        .parent = parent,         \
        .child = child,           \
}
```
那么上述有限状态机示例中的状态声明可以简化为：
```C
static const struct StateMachineState_t MyStateMachineStatus[] =
{
    FSM_ADD_STATE(IDLE_STATE, idle_entry_handler, idle_do_handler, idle_exit_handler)，
    FSM_ADD_STATE(START_STATE, start_entry_handler, start_do_handler, start_exit_handler)，
    FSM_ADD_STATE(STOP_STATE, stop_entry_handler, stop_do_handler, stop_exit_handler)，
};
```
## 2.5 事件调度
`DispatchEvent`将事件调度到状态机。

```C
enum StateMachineResultEnum DispatchEvent(struct StateMachine_t* const pStateMachine[], uint32_t quantity ...
```
该函数将一个状态机数组`pStateMachine[]`和该数组中的状态机数`quantity`作为参数。 它遍历数组中的所有状态机，如果有状态机有未处理的事件，则将事件分派给状态机。该框架支持基于优先级的事件调度，数组中状态机的索引决定状态机的优先级，索引越低优先级越高。这意味着，如果两个或更多状态机有未处理事件，则调度程序将首先调度优先级最高（数组中最低索引）的事件。

一旦事件被调度，它将不会被更高优先级的状态机抢占。处理事件后，调度程序重新从`pStateMachine[]`第一个（最高优先级）状态机开始迭代，以检查未处理事件。仅在完成当前事件处理后，才将事件分发到更高优先级的状态机。

如果事件处理时，产生自我触发（即返回`TRIGGERED_TO_SELF`），调度程序将自动调度进行自我触发事件的处理。

当状态机数组中没有任何未处理事件或未成功处理事件时，该函数返回。

>`DispatchEvent`永远不会返回 `TRIGGERED_TO_SELF`

>所有事件都处理成功，返回`EVENT_HANDLER`

>有事件未能成功处理，返回`EVENT_UN_HANDLER`

定期调用此函数，推荐使用方法是：
```C
void main()
{
    // 初始化
    while(1)
    {
        // 等待事件
        if(...)
        {
            // do something
            
            // 设置状态机的事件(例如：设置为2号事件)
            MyStateMachineStatus.event = 2;
        }            
        // 调度
        if(DispatchEvent(MyStateMachineStatus, 1) == EVENT_UN_HANDLED)
        {
            // 错误处理
        }
    }
}

```
## 2.6 状态切换
该框架支持两种类型的状态转换，`SwitchState`和`TraverseState`
```C
enum StateMachineResultEnum SwitchState(struct StateMachine_t* const pStateMachine,const struct StateMachineState_t* const pTargetState);
```
>当框架配置为为有限状态机时，使用`SwitchState`，它调用源状态的退出动作，然后调用目标状态的进入动作。

>当框架配置为为分层状态机时，如果源状态和目标状态具有共同的父级状态，也可以使用`SwitchState`。
```C
enum StateMachineResultEnum TraverseState(struct StateMachine_t* const pStateMachine,const struct StateMachineState_t* pTargetState);
```
>当框架配置为为分层状态机时，如果源状态和目标状态的父级状态不同，则必须使用`TraverseState`。它将调用源状态的每个父级状态的退出动作。然后调用每个父级状态的进入动作。
通常在状态动作（也称之为do action）中，当满足一定条件时，进行状态切换，示例如下：

```c
// State1状态的动作函数
enum StateMachineResultEnum State1DoAction(struct StateMachine_t* const sm)
{
    const struct StateMachineState_t* pTargetState;
    
    // do something
    // ...
    
    // 如果满足条件
    // if(...)
    {
        pTargetState = ...;
        return SwitchState(sm, pTargetState);
    }
}
```

## 2.7 状态机日志

该框架使用两个回调函数支持用于调试目的的日志记录机制。用户需要实现这两个回调函数的功能并将其作为参数传递给`DispatchEvent`。
```C
typedef void (*StateMachineEventLoggerHandler)(uint32_t state_machine, uint32_t state, uint32_t event);
typedef void (*StateMachineResultLoggerHandler)(uint32_t state, enum StateMachineResultEnum result);
```
**StateMachineEventLoggerHandler** 
在`DispatchEvent`将事件分派给状态机之前，将调用此函数。框架将3个参数传递给此回调函数。
- state_machine：数组中状态机的索引。
- state：状态机中当前状态的唯一ID。
- event：要调度到状态机的事件。

**StateMachineResultLoggerHandler**
在`DispatchEvent`将事件分派给状态机后，将调用此函数。框架将2个参数传递给此回调函数。
- state：事件处理后当前状态的唯一ID。
- result：状态机处理的事件的结果。

用户也可以使用此日志记录机制来记录状态机处理事件所花费的时间。在`StateMachineEventLoggerHandler`中启动计时器，然后在`StateMachineResultLoggerHandler`中停止计时器。

## 2.8 配置
可以将框架配置为支持不同的用例。要更改默认配置，请使用编译器 -D 选项定义宏`HSM_CONFIG`，并提供 hsm_config.h文件的路径。在hsm_config.h中添加您的配置，以覆盖hsm.h中的默认配置。不建议修改hsm.h进行配置。

**有限/分层状态机**

可以使用hsm_config.h文件中的宏将框架配置为支持**有限状态机**或**分层状态机**。默认情况下，框架支持分层状态机。更改`HIERARCHICAL_STATES`的值以启用/禁用**分层状态机**。

```C
// 0：禁用分层状态机。仅支持有限状态机。
// !0：启用分层状态机。有限状态机和分层状态机均受支持。
#define HIERARCHICAL_STATES    ( 0 )
```

**启用记录**

更改值`STATE_MACHINE_LOGGER`以启用/禁用状态机日志记录。默认情况下，禁用日志记录。

```C
// 0：禁用状态机记录器
// !0：使能状态机记录器
#define STATE_MACHINE_LOGGER   ( 1 )
```

**禁用可变长度数组**

`TraverseState`函数在分层状态机的实现中使用可变长度数组功能。如果由于编译器不支持或其他原因，需要禁用可变长度数组，则将`HSM_USE_VARIABLE_LENGTH_ARRAY`设置为0。默认情况下，框架可以使用可变长度数组。

```c
// 0：禁用可变长度数组
// !0：启用可变长度数组 
#define HSM_USE_VARIABLE_LENGTH_ARRAY  ( 1 )
```

**分层状态机分级数**

如果禁用“可变长度数组”，则需要手动提供所有状态机中最大分级数。用`MAX_HIERARCHICAL_LEVEL`设置最大层次级别数。例如：3层的状态机，设置`MAX_HIERARCHICAL_LEVEL`为3。
```C
// 设置分层状态机的最大分级数
#if (HSM_USE_VARIABLE_LENGTH_ARRAY == 0)
#define MAX_HIERARCHICAL_LEVEL   ( 3 )
#endif
```
