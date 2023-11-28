#include <State/entering_state.h>

void read_checksum(state_info_t* state_info) {
	uint16_t bytes_to_read = (state_info->algorithm_index == 2 ? 32 : 8) + 1; // for \r
	HAL_UART_Receive_IT(
			&huart2,
			(uint8_t*)state_info->uart_buffer,
			bytes_to_read
	);
}

void print_checksum_helper(state_info_t* state_info) {
	uint16_t bytes_to_read = (state_info->algorithm_index == 2 ? 32 : 8);
	print_uart_message(
			"\r[%u-digit checksum and Enter]: ",
			bytes_to_read
	);
}

void format_reference_checksum(state_info_t* state_info) {
	char* carry = strchr(state_info->reference_checksum, '\r');
	if (carry == NULL) {
		return;
	}

	*carry = '\0';
}

void write_enter_sum_message(state_info_t* state_info) {
	sprintf(state_info->output_buffer, "Enter reference checksum via terminal");
	format_buffer(state_info->output_buffer, TERMINAL_LINE_WIDTH);
	ST7735_DrawString(0, 0, state_info->output_buffer, Font_11x18, ST7735_BLACK, ST7735_WHITE);
}


