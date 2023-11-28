#ifndef INC_STATE_STATE_INFO_T_H_
#define INC_STATE_STATE_INFO_T_H_

#include "ST7735/fonts.h"
#include "ST7735/ST7735.h"

#include "utils.h"
#include "SD-Card/sd_card_interaction.h"

typedef enum STATE {
	IDLE,
	CHOOSE_ALGO,
	ENTER_SUM,
	EXECUTE,
	RESET_INTENT
} STATE;

typedef struct state_info_t {
	char output_buffer[DEFAULT_BUFFER_SIZE];

	STATE current_state;
	STATE state_request;
	int algorithm_index;
	uint32_t deltatime;

	int uart_write_ptr;
	char uart_buffer[DEFAULT_BUFFER_SIZE];
	char reference_checksum[MAX_CRC_LEN + 1];

	sd_card_t* sd_card;
} state_info_t;

state_info_t* new_state_info();
void free_state_info(state_info_t* state_info);

#endif /* INC_STATE_STATE_INFO_T_H_ */
