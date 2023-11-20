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

void mount_sd_card(sd_card_t* sd_card, void (*print_callback)(char* text)) {
	sd_card->fresult = f_mount(&sd_card->fs, "/", 1);
	if (sd_card->fresult != FR_OK) {
		print_callback("Error in mounting sd_card\r");
		return;
	}

	print_callback("successful mount...\r");
	f_opendir(&sd_card->directory, "/");
	DIR dir;
	FRESULT res = f_opendir(&dir, "\\FOLDER");
	if (res == FR_INT_ERR) {
		HAL_Delay(200);
		print_callback("internal error");
		HAL_Delay(200);
	}
}

FRESULT show_files(sd_card_t* sd_card, char* path, void(*print_callback)(char* text)) {
	DIR directory;
	FRESULT res = f_opendir(&directory, path);
	if (res != FR_OK) {
		HAL_Delay(200);
		print_callback("cannot open dir ");
		HAL_Delay(200);
		print_callback(path);
		return res;
	}

	HAL_Delay(200);
	print_callback("opened ");
	HAL_Delay(200);
	print_callback(path);
	HAL_Delay(200);
	print_callback("\r");
	for (;;) {
		res = f_readdir(&directory, &sd_card->file_info);
		if (res != FR_OK) {
			print_callback("error occurred!");
			HAL_Delay(200);
			break;
		}

		if (sd_card->file_info.fname[0] == 0) {
			print_callback("end of directory!");
			HAL_Delay(200);
			break;
		}

		if (sd_card->file_info.fattrib & AM_DIR) {
			UINT i = strlen(path);
			sprintf(&path[i], "\\%s", sd_card->file_info.fname);
			res = show_files(sd_card, path, print_callback);
			if (res != FR_OK) {
				break;
			}
			path[i] = 0;
		} else {
			print_callback(sd_card->file_info.fname);
			HAL_Delay(200);
		}
	}
	f_closedir(&directory);

	return res;
}

void read_next_file(sd_card_t* sd_card, void(*print_callback)(char* text)) {
	return;
}

