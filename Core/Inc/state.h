#ifndef INC_STATE_H_
#define INC_STATE_H_

#define ALGORITHMS_COUNT 3
#define SHIFT_WORD_SIZE 5
#define CHOOSE_ALGO_MSG "Choose algorithm:[NEXT/PREV]"

#include "utils.h"
#include "fonts.h"
#include "ST7735.h"

typedef enum STATE {
	IDLE,
	ENTER_SUM,
	CHOOSE_ALGO,
	EXECUTE,
	OUTPUT
} STATE;

static const char* algorithms[] = {
		"first      ",
		"second     ",
		"third      "
};

typedef struct state_info_t {
	char output_buffer[DEFAULT_BUFFER_SIZE];

	STATE current_state;
	int algorithm_index;
	int deltatime;

	int uart_write_ptr;
	char uart_buffer[DEFAULT_BUFFER_SIZE];
	char reference_checksum[DEFAULT_BUFFER_SIZE];
} state_info_t;

state_info_t* new_state_info();
void set_state(state_info_t* state_info, STATE new_state);
void set_next_algo(state_info_t* state_info);
void set_prev_algo(state_info_t* state_info);
void reduce_state_to_constant_output(state_info_t* state_info);
void reduce_state_change_to_action(state_info_t* state_info);
void write_enter_sum_message(state_info_t* state_info);
void write_algorithm_message(state_info_t* state_info);
void write_algorithm_name(state_info_t* state_info);
void write_execution_time(state_info_t* state_info);

#endif /* INC_STATE_H_ */
