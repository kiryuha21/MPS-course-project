#include <State/execution_state.h>

void process_execution(state_info_t* state_info) {
	uint32_t start = HAL_GetTick();
	ALGORITHM algo = get_algo_from_index(state_info->algorithm_index);
	set_algorithm(state_info->sd_card->algorithm_ctx, algo);
	calculate_checksum(state_info->sd_card);
	algorithm_finalize(state_info->sd_card->algorithm_ctx);
	state_info->deltatime = HAL_GetTick() - start;
}

char* extract_result(state_info_t* state_info) {
	return state_info->sd_card->algorithm_ctx->result;
}

void write_checksum_report(state_info_t* state_info) {
	char* result = extract_result(state_info);
	bool is_match = strcmp(state_info->reference_checksum, extract_result(state_info)) == 0;

	sprintf(state_info->output_buffer, "Checksums %sequal",	is_match ? "" : "not ");
	format_buffer(state_info->output_buffer, TERMINAL_LINE_WIDTH);
	ST7735_DrawString(0, 0, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);

	uint16_t y1 = TERMINAL_LINE_HEIGHT * (is_match ? 2 : 3);
	ST7735_DrawString(0, y1, "Checksum:", Font_11x18, ST7735_BLACK, ST7735_WHITE);
	ST7735_DrawString(0, y1 + TERMINAL_LINE_HEIGHT, result, Font_7x10, ST7735_BLACK, ST7735_WHITE);

	uint16_t y2 = y1 + TERMINAL_LINE_HEIGHT * 2;
	clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);
	sprintf(state_info->output_buffer, "Executed in %lu ms", state_info->deltatime);
	ST7735_DrawString(0, y2, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);

	if (!is_match) {
		make_error_sound();
	}
}

ALGORITHM get_algo_from_index(int algorithm_index) {
	switch (algorithm_index) {
	case 0:
		return HAL_CRC;
	case 1:
		return CRC8;
	case 2:
		return MD5;
	default:
		print_uart_message("this shouldn't happen\r");
		return HAL_CRC;
	}
}

void make_error_sound() {
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_Delay(1000);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}


