#include <State/choosing_state.h>

static const char* algorithms[] = {
		"Native CRC",
		"Software CRC",
		"Software MD5"
};

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

void read_algorithm_shift(state_info_t* state_info) {
	HAL_UART_Receive_IT(
			&huart2,
			(uint8_t*)state_info->uart_buffer,
			SHIFT_WORD_SIZE
	);
}

void get_uart_input(state_info_t* state_info) {
	if (state_info->current_state != CHOOSE_ALGO) {
		return;
	}

	read_algorithm_shift(state_info);
}
