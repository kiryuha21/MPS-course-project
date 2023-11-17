#include "sd_card_interaction.h"

sd_card_t* new_sd_card() {
	sd_card_t* sd_card = calloc(1, sizeof(sd_card_t));
	memset(sd_card->buffer, 0, DEFAULT_BUFFER_SIZE);
	return sd_card;
}

void mount_sd_card(sd_card_t* sd_card, void (*print_callback)(char* text)) {
	sd_card->fresult = f_mount(&sd_card->fs, "/", 1);
	if (sd_card->fresult != FR_OK) {
		print_callback("Error in mounting sd_card\r");
		return;
	}

	print_callback("successful mount...\r");
	f_opendir(&sd_card->directory, "/");
}

void show_files(sd_card_t* sd_card, void(*print_callback)(char* text)) {
	while (f_readdir(&sd_card->directory, &sd_card->file_info) == FR_OK) {
		if (sd_card->file_info.fname[0] == 0) {
			print_callback("end of directory\n");
			break;
		}

		print_callback(sd_card->file_info.fname);
		HAL_Delay(200);
		print_callback("\r");
		HAL_Delay(200);
	}
}

void read_next_file(sd_card_t* sd_card, void(*print_callback)(char* text)) {
	static bool dir_opened = false;
	static DIR dir;

	if (dir_opened == false) {
		f_opendir(&dir, "/");
		dir_opened = true;
		f_findnext(&dir, &sd_card->file_info);
	} else {
		f_findnext(&dir, &sd_card->file_info);
	}

	print_callback(sd_card->file_info.fname);
}

