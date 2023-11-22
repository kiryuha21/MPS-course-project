#include "Algorithms/algorithms.h"

algorithm_ctx_t* new_algorithm_context() {
	algorithm_ctx_t* ctx = calloc(1, sizeof(algorithm_ctx_t));
	ctx->current_value = 0;
	clear_buffer(ctx->result, MAX_CRC_LEN);
	return ctx;
}

void set_algorithm(algorithm_ctx_t* ctx, ALGORITHM algo) {
	ctx->algorithm = algo;
}

void algorithm_init(algorithm_ctx_t* ctx, char* buffer) {
	switch (ctx->algorithm) {
	case MD5:
		md5Init(&ctx->md5ctx);
		md5Update(&ctx->md5ctx, (uint8_t*)buffer, DEFAULT_BUFFER_SIZE);
		break;
	case CRC8:
		break;
	case HAL_CRC:
		HAL_CRC_Calculate(&hcrc, (uint32_t*)buffer, DEFAULT_BUFFER_SIZE);
		break;
	}
}

void algorithm_update(algorithm_ctx_t* ctx, char* buffer) {
	switch (ctx->algorithm) {
	case MD5:
		md5Update(&ctx->md5ctx, (uint8_t*)buffer, DEFAULT_BUFFER_SIZE);
		break;
	case CRC8:
		break;
	case HAL_CRC:
		ctx->current_value = HAL_CRC_Accumulate(&hcrc, (uint32_t*)buffer, DEFAULT_BUFFER_SIZE);
		break;
	}
}

void algorithm_finalize(algorithm_ctx_t* ctx) {
	switch (ctx->algorithm) {
	case MD5:
		md5Finalize(&ctx->md5ctx);
		memcpy(ctx->result, ctx->md5ctx.digest, 16);
		break;
	case CRC8:
		break;
	case HAL_CRC:
		sprintf(ctx->result, "%u", ctx->current_value);
		break;
	}
}


