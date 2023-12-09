#ifndef INC_STATE_EXECUTION_STATE_H_
#define INC_STATE_EXECUTION_STATE_H_

#include "State/state_info_t.h"
#include "Algorithms/algorithms.h"

extern TIM_HandleTypeDef htim2;

void process_execution(state_info_t* state_info);
char* extract_result(state_info_t* state_info);
void write_checksum_report(state_info_t* state_info);
ALGORITHM get_algo_from_index(int algorithm_index);
void make_error_sound();

#endif /* INC_STATE_EXECUTION_STATE_H_ */
