#include "State/state.h"

void set_state(state_info_t* state_info, STATE new_state) {
	state_info->current_state = new_state;
	reduce_state_change_to_effect(state_info);
}

void reset_state(state_info_t* state_info) {
	set_state(state_info, CHOOSE_ALGO);
	reset_calculation(state_info->sd_card);
	state_info->algorithm_index = 0;
}

void check_state_request(state_info_t* state_info) {
	if (state_info->state_request == IDLE) {
		return;
	}

	set_state(state_info, state_info->state_request);
	state_info->state_request = IDLE;
}

void reduce_state_to_action(state_info_t* state_info) {
	if (state_info->current_state != CHOOSE_ALGO) {
		return;
	}

	write_algorithm_name(state_info);
}

void reduce_state_change_to_effect(state_info_t* state_info) {
	switch (state_info->current_state) {
	case CHOOSE_ALGO:
		ST7735_FillScreen(ST7735_WHITE);
		clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);
		write_algorithm_message(state_info);
		print_uart_message("%s", CHOOSE_ALGO_MSG);
		break;
	case ENTER_SUM:
		ST7735_FillScreen(ST7735_WHITE);
		clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);
		write_enter_sum_message(state_info);
		print_checksum_helper(state_info);
		read_checksum(state_info);
		break;
	case EXECUTE:
		ST7735_FillScreen(ST7735_WHITE);
		clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);
		process_execution(state_info);
		write_checksum_report(state_info);
		break;
	case RESET_INTENT:
		reset_state(state_info);
		break;
	default:
		print_uart_message("this shouldn't happen\r");
		break;
	}
}
