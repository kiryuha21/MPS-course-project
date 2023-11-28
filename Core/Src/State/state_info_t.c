#include "State/state_info_t.h"

state_info_t* new_state_info() {
	state_info_t* state_info = calloc(1, sizeof(state_info_t));

	clear_buffer(state_info->output_buffer, DEFAULT_BUFFER_SIZE);

	state_info->algorithm_index = 0;
	state_info->current_state = IDLE;
	state_info->state_request = IDLE;
	state_info->deltatime = 0;

	state_info->uart_write_ptr = 0;
	clear_buffer(state_info->uart_buffer, DEFAULT_BUFFER_SIZE);
	clear_buffer(state_info->reference_checksum, MAX_CRC_LEN + 1);

	state_info->sd_card = new_sd_card();
	mount_sd_card(state_info->sd_card);

	return state_info;
}

void free_state_info(state_info_t* state_info) {
	free_sd_card(state_info->sd_card);
	free(state_info);
}

