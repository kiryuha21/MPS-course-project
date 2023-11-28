#ifndef INC_STATE_CHOOSING_STATE_H_
#define INC_STATE_CHOOSING_STATE_H_

#include "State/state_info_t.h"
#include "Algorithms/algorithms.h"

#include "ST7735/fonts.h"
#include "ST7735/ST7735.h"

#define ALGORITHMS_COUNT 3
#define SHIFT_WORD_SIZE 5
#define CHOOSE_ALGO_MSG "Choose algorithm[NEXT/PREV]: "

void set_next_algo(state_info_t* state_info);
void set_prev_algo(state_info_t* state_info);
void write_algorithm_message(state_info_t* state_info);
void write_algorithm_name(state_info_t* state_info);
void read_algorithm_shift(state_info_t* state_info);
void get_uart_input(state_info_t* state_info);

#endif /* INC_STATE_CHOOSING_STATE_H_ */
