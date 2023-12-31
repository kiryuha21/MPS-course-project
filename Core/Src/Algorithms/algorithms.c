#include "Algorithms/algorithms.h"

algorithm_ctx_t* new_algorithm_context() {
	algorithm_ctx_t* ctx = calloc(1, sizeof(algorithm_ctx_t));
	ctx->current_value = 0;
	clear_buffer(ctx->result, MAX_CRC_LEN + 1);
	return ctx;
}

void set_algorithm(algorithm_ctx_t* ctx, ALGORITHM algo) {
	ctx->algorithm = algo;
}

void format_md5_hash(char* dest, uint8_t* src) {
	for (int i = 0; i < 16; ++i) {
		sprintf(dest + i * 2, "%02x", src[i]);
	}
}

void algorithm_init(algorithm_ctx_t* ctx, char* buffer) {
	switch (ctx->algorithm) {
	case MD5:
		md5Init(&ctx->md5ctx);
		md5Update(&ctx->md5ctx, (uint8_t*)buffer, DEFAULT_BUFFER_SIZE);
		break;
	case CRC8:
		ctx->current_value = crc8(0, NULL, 0);
		ctx->current_value = crc8(ctx->current_value, (unsigned char*)buffer, DEFAULT_BUFFER_SIZE);
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
		ctx->current_value = crc8(ctx->current_value, (unsigned char*)buffer, DEFAULT_BUFFER_SIZE);
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
		format_md5_hash(ctx->result, ctx->md5ctx.digest);
		break;
	case CRC8:
		sprintf(ctx->result, "%08lx", ctx->current_value);
		break;
	case HAL_CRC:
		sprintf(ctx->result, "%08lx", ctx->current_value);
		break;
	}
}


