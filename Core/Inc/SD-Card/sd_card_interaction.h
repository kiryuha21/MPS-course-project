#ifndef INC_SD_CARD_INTERACTION_H_
#define INC_SD_CARD_INTERACTION_H_

#include "fatfs_sd.h"
#include "fatfs.h"
#include "utils.h"

extern CRC_HandleTypeDef hcrc;

typedef struct sd_card_t {
	FATFS fs;  // file system
	FIL file; // file
	DIR directory;
	FILINFO file_info; // file info
	FRESULT fresult;  // result

	char buffer[DEFAULT_BUFFER_SIZE];
} sd_card_t;

sd_card_t* new_sd_card();
void free_sd_card(sd_card_t* sd_card);
void mount_sd_card(sd_card_t* sd_card, void (*print_callback)(char* format, ...));
unsigned int show_files(sd_card_t* sd_card, void(*print_callback)(char* format, ...));
void read_next_file(sd_card_t* sd_card, void(*print_callback)(char* format, ...));

#endif /* INC_SD_CARD_INTERACTION_H_ */
