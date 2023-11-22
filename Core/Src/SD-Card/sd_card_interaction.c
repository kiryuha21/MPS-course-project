#include "SD-Card/sd_card_interaction.h"

sd_card_t* new_sd_card() {
	sd_card_t* sd_card = calloc(1, sizeof(sd_card_t));
	memset(sd_card->buffer, 0, DEFAULT_BUFFER_SIZE);
	return sd_card;
}

void free_sd_card(sd_card_t* sd_card) {
	f_mount(NULL, "", 1);
	free(sd_card);
}

void mount_sd_card(sd_card_t* sd_card, void (*print_callback)(char* format, ...)) {
	sd_card->fresult = f_mount(&sd_card->fs, "/", 1);
	if (sd_card->fresult != FR_OK) {
		print_callback("Error in mounting sd_card\r");
		return;
	}

	print_callback("successful mount...\r");
}

unsigned int hash_next_file(char* filename, void(*print_callback)(char* format, ...)) {
	static bool hash_initialized = false;

	FIL file;
	unsigned int res = f_open(&file, filename, FA_READ);
	if (res != 0) {
		print_callback("filename is %s\r", filename);
		print_callback("something went wrong!");
		return res;
	}

	UINT br;
	char buffer[DEFAULT_BUFFER_SIZE];
	uint32_t res_buffer[DEFAULT_BUFFER_SIZE];
	while (true) {
		f_read(&file, buffer, DEFAULT_BUFFER_SIZE, &br);
		if (br == 0) {
			break;
		}
		char_array_to_uint32_array(buffer, res_buffer, DEFAULT_BUFFER_SIZE);
		if (!hash_initialized) {
			res = HAL_CRC_Calculate(&hcrc, res_buffer, DEFAULT_BUFFER_SIZE);
		} else {
			res = HAL_CRC_Accumulate(&hcrc, res_buffer, DEFAULT_BUFFER_SIZE);
		}
	}
	print_callback("\rbuffer is '%s'\r", buffer);
	print_callback("%\r%lu\r", res);

	f_close(&file);
	return res;
}

unsigned int show_files(sd_card_t* sd_card, void(*print_callback)(char* format, ...)) {
	static char path[DEFAULT_BUFFER_SIZE] = {0};
	DIR directory;
	unsigned int res = f_opendir(&directory, path);
	if (res != FR_OK) {
		print_callback("\rcannot open dir ");
		print_callback(path);
		return res;
	}

	print_callback("\ropened ");
	print_callback(path);
	for (;;) {
		res = f_readdir(&directory, &sd_card->file_info);
		if (res != FR_OK) {
			print_callback("error occurred!");
			break;
		}

		if (sd_card->file_info.fname[0] == 0) {
			print_callback("\rend of directory!");
			break;
		}

		if (sd_card->file_info.fattrib & AM_DIR) {
			UINT i = strlen(path);
			sprintf(&path[i], "\\%s", sd_card->file_info.fname);
			res = show_files(sd_card, print_callback);
			if (res != FR_OK) {
				break;
			}
			path[i] = 0;
		} else { // is file
			char full_name[DEFAULT_BUFFER_SIZE] = {0};
			sprintf(full_name, "%s\\%s", path, sd_card->file_info.fname);
			print_callback("\r");
			print_callback(sd_card->file_info.fname);
			res = hash_next_file(full_name, print_callback);
		}
	}
	f_closedir(&directory);

	return res;
}

