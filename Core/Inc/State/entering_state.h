#ifndef INC_STATE_ENTERING_STATE_H_
#define INC_STATE_ENTERING_STATE_H_

#include <State/state_info_t.h>

void read_checksum(state_info_t* state_info);
void print_checksum_helper(state_info_t* state_info);
void format_reference_checksum(state_info_t* state_info);
void write_enter_sum_message(state_info_t* state_info);

#endif /* INC_STATE_ENTERING_STATE_H_ */
