/*
Copyright Nomagno 2023

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/
#ifndef H5STATE_H
#define H5STATE_H

#include <halfive/h5req.h>
#include <halfive/h5vm/h5vm.h>

typedef struct {
	_Bool call;
	unsigned previous_state;
	unsigned current_state;
	unsigned next_state;
	unsigned remaining_time;
} H5State_StateMachine;

typedef enum {
	enum_typeUnknown = 0,
	enum_typeThinker = 1,
	enum_typeVFX = 2
} H5State_EntityType;

typedef struct {
	_Bool exists;
	H5State_EntityType entity_type;
	H5State_StateMachine state_machine;

	char data_type;
	_Bool data_is_allocated;
	void *data;
} H5State_Entity;

typedef struct {
	h5uint data[16];
} H5State_EntityBasicData_Raw;

typedef struct {
	h5uint prev_state;
	h5uint next_state;

	h5uint axis_1_2; /*11111111 22222222*/
	h5uint button_mask; /* UP DOWN LEFT RIGHT A B X Y L R ZL ZR MOUSE1 MOUSE2 IS_CURSOR_ON_SELF IS_JUST_CLICKED_MOUSE1*/

	h5uint pos_x;
	h5uint pos_y;
	h5uint rot;

	h5uint nearby_entities_flag_1_2_3; /* ff 111111 222222 33 */
	h5uint nearby_entities_3_4_5;      /* 3333 444444 555555 */
	h5uint nearby_entities_flag_types; /* f 111 222 333 444 555*/

	h5uint custom_data[6];
} H5State_EntityBasicData_BasicOverlay;


typedef H5State_Entity(*H5State_NativeStateTransition)(H5State_Entity);

typedef enum {
	enum_transitionDisabled = 0,
	enum_transitionNative = 1,
	enum_transitionBinary = 2,
	enum_transitionBoth = 3,
} H5State_TransitionType;

typedef struct {
	H5State_TransitionType type;
	H5State_NativeStateTransition native_trans;
	H5VM_CrossProcessProgramCounter binary_trans;
} H5State_StateTransition;

typedef struct {
	H5State_StateTransition on_entry;
	H5State_StateTransition update;
	H5State_StateTransition on_exit;
} H5State_State;

typedef struct {
	H5State_State states[128];
	H5State_Entity entities[64];
	H5State_EntityBasicData_Raw entity_raw_data[64]; /*To be mapped as the VM's process memory for VM states*/
	unsigned current_tick;
} H5State_GlobalObjectManagementTable;

H5State_EntityBasicData_BasicOverlay H5State_convertRawToBasic(H5State_EntityBasicData_Raw raw);
H5State_EntityBasicData_Raw H5State_convertBasicToRaw(H5State_EntityBasicData_BasicOverlay raw);

void H5State_simulateOneTick(H5State_GlobalObjectManagementTable *table);
#endif
