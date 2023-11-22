#include "state.h"
#include "utils.h"

state_info_t* new_state_info() {
	state_info_t* state_info = calloc(1, sizeof(state_info_t));

	clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);

	state_info->algorithm_index = 0;
	state_info->current_state = IDLE;
	state_info->deltatime = 0;

	state_info->uart_write_ptr = 0;
	clear_buffer(state_info->uart_buffer, DEFAULT_BUFFER_SIZE);
	clear_buffer(state_info->reference_checksum, DEFAULT_BUFFER_SIZE);

	state_info->sd_card = new_sd_card();
	mount_sd_card(state_info->sd_card);

//	uint32_t start = HAL_GetTick();
//	set_algorithm(state_info->sd_card->algorithm_ctx, MD5);
//	calculate_checksum(state_info->sd_card);
//	state_info->deltatime = HAL_GetTick() - start;
//	print_uart_message("deltatime is %lu\r", state_info->deltatime);
//	print_uart_message("checksum is %u\r", state_info->sd_card->algorithm_ctx->result);

	return state_info;
}

void free_state_info(state_info_t* state_info) {
	free_sd_card(state_info->sd_card);
	free(state_info);
}

void set_state(state_info_t* state_info, STATE new_state) {
	state_info->current_state = new_state;
	if (new_state == EXECUTE) {
		print_uart_message("execution\r");
	}
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

void process_execution(state_info_t* state_info) {
	uint32_t start = HAL_GetTick();
	ALGORITHM algo = get_algo_from_index(state_info->algorithm_index);
	set_algorithm(state_info->sd_card->algorithm_ctx, algo);
	calculate_checksum(state_info->sd_card);
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
	sprintf(
			state_info->output_buffer, "Checksums %sequal, executed in %lu ms",
			checksums_equal(state_info) ? "" : "not ",
			state_info->deltatime
	);
	format_buffer(state_info->output_buffer, TERMINAL_LINE_WIDTH);
	ST7735_DrawString(0, 0, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);
}

