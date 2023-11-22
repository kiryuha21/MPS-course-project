#ifndef INC_SD_CARD_INTERACTION_H_
#define INC_SD_CARD_INTERACTION_H_

#include "fatfs_sd.h"
#include "fatfs.h"
#include "utils.h"
#include "Algorithms/algorithms.h"

extern CRC_HandleTypeDef hcrc;

typedef struct sd_card_t {
	FATFS fs;  // file system
	FIL file; // file
	FILINFO file_info; // file info

	algorithm_ctx_t* algorithm_ctx;
	bool algorithm_initialized;
} sd_card_t;

sd_card_t* new_sd_card();
void free_sd_card(sd_card_t* sd_card);
void mount_sd_card(sd_card_t* sd_card);
void reset_calculation(sd_card_t* sd_card);
FRESULT calculate_checksum(sd_card_t* sd_card);

#endif /* INC_SD_CARD_INTERACTION_H_ */
