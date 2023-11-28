#ifndef INC_STATE_H_
#define INC_STATE_H_

#include "State/state_info_t.h"
#include "State/choosing_state.h"
#include "State/entering_state.h"
#include "State/execution_state.h"

void check_state_request(state_info_t* state_info);
void set_state(state_info_t* state_info, STATE new_state);
void reset_state(state_info_t* state_info);
void reduce_state_to_action(state_info_t* state_info);
void reduce_state_change_to_effect(state_info_t* state_info);

#endif /* INC_STATE_H_ */
