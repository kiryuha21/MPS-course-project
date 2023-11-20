#include "state.h"
#include "utils.h"

state_info_t* new_state_info() {
	state_info_t* state_info = calloc(1, sizeof(state_info_t));

	clear_buffer(state_info->output_buffer);

	state_info->algorithm_index = 0;
	state_info->current_state = IDLE;
	state_info->deltatime = 0;

	state_info->uart_write_ptr = 0;
	clear_buffer(state_info->uart_buffer);
	clear_buffer(state_info->reference_checksum);

	return state_info;
}

void set_state(state_info_t* state_info, STATE new_state) {
	state_info->current_state = new_state;
	reduce_state_change_to_action(state_info);
}

void set_next_algo(state_info_t* state_info) {
	state_info->algorithm_index += 1;
	state_info->algorithm_index %= ALGORITHMS_COUNT;
}

void set_prev_algo(state_info_t* state_info) {
	state_info->algorithm_index -= 1;
	if (state_info->algorithm_index == -1) {
		state_info->algorithm_index = ALGORITHMS_COUNT - 1;
	}
}

void reduce_state_to_constant_output(state_info_t* state_info) {
	if (state_info->current_state != CHOOSE_ALGO) {
		return;
	}

	write_algorithm_name(state_info);
}

void reduce_state_change_to_action(state_info_t* state_info) {
	switch (state_info->current_state) {
	case ENTER_SUM:
		ST7735_FillScreen(ST7735_WHITE);
		write_enter_sum_message(state_info);
		break;
	case CHOOSE_ALGO:
		ST7735_FillScreen(ST7735_WHITE);
		write_algorithm_message(state_info);
		break;
	case EXECUTE:
		ST7735_FillScreen(ST7735_WHITE);
		clear_buffer(state_info->output_buffer);
		break;
	case OUTPUT:
		ST7735_FillScreen(ST7735_WHITE);
		write_execution_time(state_info);
		break;
	default:
		break;
	}
}

void write_enter_sum_message(state_info_t* state_info) {
	sprintf(state_info->output_buffer, "Enter reference checksum via terminal");
	format_buffer(state_info->output_buffer, TERMINAL_LINE_WIDTH);
	ST7735_DrawString(0, 0, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);
}

void write_algorithm_message(state_info_t* state_info) {
	sprintf(state_info->output_buffer, "Current algorithm:");
	format_buffer(state_info->output_buffer, TERMINAL_LINE_WIDTH);
	ST7735_DrawString(0, 0, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);
}

void write_algorithm_name(state_info_t* state_info) {
	uint16_t y = TERMINAL_LINE_HEIGHT * 2;
	sprintf(state_info->output_buffer, algorithms[state_info->algorithm_index]);
	format_buffer(state_info->output_buffer, TERMINAL_LINE_WIDTH);
	ST7735_DrawString(0, y, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);
}

void write_execution_time(state_info_t* state_info) {

}

