#include "state.h"
#include "utils.h"

state_info_t* new_state_info() {
	state_info_t* state_info = calloc(1, sizeof(state_info_t));

	clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);

	state_info->algorithm_index = 0;
	state_info->current_state = IDLE;
	state_info->current_state = IDLE;
	state_info->deltatime = 0;

	state_info->uart_write_ptr = 0;
	clear_buffer(state_info->uart_buffer, DEFAULT_BUFFER_SIZE);
	clear_buffer(state_info->reference_checksum, DEFAULT_BUFFER_SIZE);

	state_info->sd_card = new_sd_card();
	mount_sd_card(state_info->sd_card);

	return state_info;
}

void free_state_info(state_info_t* state_info) {
	free_sd_card(state_info->sd_card);
	free(state_info);
}

void check_state_request(state_info_t* state_info) {
	if (state_info->state_request == IDLE) {
		return;
	}

	set_state(state_info, state_info->state_request);
	state_info->state_request = IDLE;
}

void set_state(state_info_t* state_info, STATE new_state) {
	state_info->current_state = new_state;
	reduce_state_change_to_effect(state_info);
}

void reset_state(state_info_t* state_info) {
	set_state(state_info, ENTER_SUM);
	reset_calculation(state_info->sd_card);
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

void reduce_state_to_action(state_info_t* state_info) {
	if (state_info->current_state != CHOOSE_ALGO) {
		return;
	}

	write_algorithm_name(state_info);
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

char* hash_result(state_info_t* state_info) {
	return state_info->sd_card->algorithm_ctx->result;
}

void process_execution(state_info_t* state_info) {
	uint32_t start = HAL_GetTick();
	ALGORITHM algo = get_algo_from_index(state_info->algorithm_index);
	set_algorithm(state_info->sd_card->algorithm_ctx, algo);
	calculate_checksum(state_info->sd_card);
	algorithm_finalize(state_info->sd_card->algorithm_ctx);
	state_info->deltatime = HAL_GetTick() - start;
}

bool checksums_equal(state_info_t* state_info) {
	return strcmp(
			state_info->reference_checksum,
			state_info->sd_card->algorithm_ctx->result
	) == 0;
}

void reduce_state_change_to_effect(state_info_t* state_info) {
	switch (state_info->current_state) {
	case ENTER_SUM:
		ST7735_FillScreen(ST7735_WHITE);
		clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);
		write_enter_sum_message(state_info);
		break;
	case CHOOSE_ALGO:
		ST7735_FillScreen(ST7735_WHITE);
		clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);
		write_algorithm_message(state_info);
		break;
	case EXECUTE:
		ST7735_FillScreen(ST7735_WHITE);
		clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);
		process_execution(state_info);
		write_checksum_report(state_info);
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

void write_checksum_report(state_info_t* state_info) {
	char* result = hash_result(state_info);
	bool is_match = checksums_equal(state_info);

	sprintf(state_info->output_buffer, "Checksums %sequal",	is_match ? "" : "not ");
	format_buffer(state_info->output_buffer, TERMINAL_LINE_WIDTH);
	ST7735_DrawString(0, 0, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);

	uint16_t y1 = TERMINAL_LINE_HEIGHT * (is_match ? 2 : 3);
	ST7735_DrawString(0, y1, "Checksum:", Font_11x18, ST7735_BLACK, ST7735_WHITE);
	ST7735_DrawString(0, y1 + TERMINAL_LINE_HEIGHT * 1.25, result, Font_7x10, ST7735_BLACK, ST7735_WHITE);

	uint16_t y2 = y1 + TERMINAL_LINE_HEIGHT * 2;
	clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);
	sprintf(state_info->output_buffer, "Executed in %lu ms", state_info->deltatime);
	ST7735_DrawString(0, y2, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);
}

