#ifndef INC_ALGORITHMS_ALGORITHMS_H_
#define INC_ALGORITHMS_ALGORITHMS_H_

#include "Algorithms/crc8.h"
#include "Algorithms/md5.h"
#include "utils.h"

#define MAX_CRC_LEN 16

extern CRC_HandleTypeDef hcrc;

typedef enum ALGORITHM {
	MD5,
	CRC8,
	HAL_CRC
} ALGORITHM;

typedef struct algorithm_ctx {
	ALGORITHM algorithm;
	MD5Context md5ctx;

	unsigned int current_value;
	char result[MAX_CRC_LEN];
} algorithm_ctx_t;

algorithm_ctx_t* new_algorithm_context();
void set_algorithm(algorithm_ctx_t* ctx, ALGORITHM algo);
void algorithm_init(algorithm_ctx_t* ctx, char* buffer);
void algorithm_update(algorithm_ctx_t* ctx, char* buffer);
void algorithm_finalize(algorithm_ctx_t* ctx);

#endif /* INC_ALGORITHMS_ALGORITHMS_H_ */
