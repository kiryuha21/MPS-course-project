#include "SD-Card/sd_card_interaction.h"

sd_card_t* new_sd_card() {
	sd_card_t* sd_card = calloc(1, sizeof(sd_card_t));
	sd_card->algorithm_ctx = new_algorithm_context();
	sd_card->algorithm_initialized = false;
	return sd_card;
}

void free_sd_card(sd_card_t* sd_card) {
	free(sd_card->algorithm_ctx);
	f_mount(NULL, "", 1);
	free(sd_card);
}

void reset_calculation(sd_card_t* sd_card) {
	sd_card->algorithm_initialized = false;
}

void mount_sd_card(sd_card_t* sd_card) {
	FRESULT fres = f_mount(&sd_card->fs, "/", 1);
	if (fres != FR_OK) {
		print_uart_message("Error in mounting sd_card\r");
		return;
	}

	print_uart_message("successful mount...\r");
}

FRESULT hash_next_file(
		char* filename,
		algorithm_ctx_t* ctx,
		void (*algorithm_function)(algorithm_ctx_t*, char*)
) {
	FIL file;
	FRESULT fres = f_open(&file, filename, FA_READ);
	if (fres != FR_OK) {
		print_uart_message("something went wrong with %s\r", filename);
		return fres;
	}

	UINT br;
	char buffer[DEFAULT_BUFFER_SIZE] = {0};
	while (true) {
		f_read(&file, buffer, DEFAULT_BUFFER_SIZE, &br);
		if (br == 0) {
			break;
		}
		// print_uart_message("%s\r", buffer);
		algorithm_function(ctx, buffer);
	}

	f_close(&file);
	return FR_OK;
}

FRESULT calculate_checksum(sd_card_t* sd_card) {
	static char path[DEFAULT_BUFFER_SIZE] = {0};
	DIR directory;

	FRESULT fres = f_opendir(&directory, path);
	if (fres != FR_OK) {
		print_uart_message("\rcannot open dir %s\r", path);
		return fres;
	}

	// print_callback("\ropened %s", path);
	while (true) {
		fres = f_readdir(&directory, &sd_card->file_info);
		if (fres != FR_OK) {
			print_uart_message("error occurred!\r");
			break;
		}

		if (sd_card->file_info.fname[0] == 0) {
			// print_callback("\rend of directory!");
			break;
		}

		if (sd_card->file_info.fattrib & AM_DIR) {
			UINT i = strlen(path);
			sprintf(&path[i], "\\%s", sd_card->file_info.fname);
			fres = calculate_checksum(sd_card);
			if (fres != FR_OK) {
				print_uart_message("error occurred!");
				return FR_INT_ERR;
			}
			path[i] = 0;
		} else { // is file
			char full_name[DEFAULT_BUFFER_SIZE] = {0};
			sprintf(full_name, "%s\\%s", path, sd_card->file_info.fname);
			if (sd_card->algorithm_initialized) {
				hash_next_file(full_name, sd_card->algorithm_ctx, algorithm_update);
			} else {
				hash_next_file(full_name, sd_card->algorithm_ctx, algorithm_init);
				sd_card->algorithm_initialized = true;
			}
		}
	}

	f_closedir(&directory);
	return FR_OK;
}

