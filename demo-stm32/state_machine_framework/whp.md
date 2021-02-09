#根据代码规范修改内容

对所有struct禁止使用typedef

| 原来的代码 | 修改后 |
| ------ | ------ |
|state_handler|StateHandler|
|Handler|handler|
|Entry| entry|
|Exit|exit|
|Id|id|
|Parent|parent|
|Node|child|
|Level|level|
|state_t|strcut StateMachineState_t|
|hierarchical_state|去除|
|finite_state|去除|
|state_machine_t|strcut StateMachine_t|
|Event|event|
|State|state|
|state_machine_result_t|enum StateMachineResultEnum|
|state_machine_event_logger|StateMachineEventLoggerHandler|
|state_machine_result_logger|StateMachineResultLoggerHandler|
|dispatch_event|DispatchEvent|
|switch_state|SwitchState|
|traverse_state|TraverseState|
|pState_Machine|pStateMachine|
|pTarget_State|pTargetState|
|pTarget_Path|pTargetPath|
