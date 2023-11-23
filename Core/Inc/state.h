#ifndef INC_STATE_H_
#define INC_STATE_H_

#define ALGORITHMS_COUNT 3
#define SHIFT_WORD_SIZE 5
#define CHOOSE_ALGO_MSG "Choose algorithm:[NEXT/PREV]"

#include "utils.h"
#include "ST7735/fonts.h"
#include "ST7735/ST7735.h"
#include "SD-Card/sd_card_interaction.h"

typedef enum STATE {
	IDLE,
	ENTER_SUM,
	CHOOSE_ALGO,
	EXECUTE,
	RESET_INTENT
} STATE;

static const char* algorithms[] = {
		"Native CRC",
		"Software CRC",
		"Software MD5"
};

typedef struct state_info_t {
	char output_buffer[DEFAULT_BUFFER_SIZE];

	STATE current_state;
	STATE state_request;
	int algorithm_index;
	uint32_t deltatime;

	int uart_write_ptr;
	char uart_buffer[DEFAULT_BUFFER_SIZE];
	char reference_checksum[MAX_CRC_LEN];

	sd_card_t* sd_card;
} state_info_t;

state_info_t* new_state_info();
void free_state_info(state_info_t* state_info);

void check_state_request(state_info_t* state_info);
void set_state(state_info_t* state_info, STATE new_state);
void reset_state(state_info_t* state_info);
void set_next_algo(state_info_t* state_info);
void set_prev_algo(state_info_t* state_info);

void reduce_state_to_action(state_info_t* state_info);
void reduce_state_change_to_effect(state_info_t* state_info);

void write_enter_sum_message(state_info_t* state_info);
void write_algorithm_message(state_info_t* state_info);
void write_algorithm_name(state_info_t* state_info);
void write_checksum_report(state_info_t* state_info);

#endif /* INC_STATE_H_ */
