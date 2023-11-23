#include "utils.h"

void format_buffer(char* buffer, size_t line_width) {
	char res[DEFAULT_BUFFER_SIZE] = {'\0'};

	size_t shift = 0;
	size_t len = strlen(buffer);
	for (size_t i = 0; i < len; ++i) {
		if (buffer[i] != ' ') {
			res[i + shift] = buffer[i];
			continue;
		}

		size_t spaces_amount = line_width - ((i + shift) % line_width);
		for (size_t j = 0; j < spaces_amount; ++j) {
			res[i + j + shift] = ' ';
		}
		shift += spaces_amount - 1;
	}

	strcpy(buffer, res);
}

void clear_buffer(char* buffer, size_t size) {
	memset(buffer, '\0', size);
}

void char_array_to_uint32_array(char* src, uint32_t* dest, int len) {
	for (int i = 0; i < len; ++i) {
		dest[i] = (uint32_t)src[i];
	}
}

void uint32_array_to_char_array(uint32_t* src, char* dest, int len) {
	for (int i = 0; i < len; ++i) {
		dest[i] = (char)src[i];
	}
}

void to_lower(char* string) {
	size_t len = strlen(string);
	for (size_t i = 0; i < len; ++i) {
		string[i] = tolower(string[i]);
	}
}

void print_uart_message(char* format, ...) {
	va_list args;
	char res[DEFAULT_BUFFER_SIZE]={0};
	va_start(args, format);
	vsprintf(res, format, args);
	va_end(args);

	HAL_UART_Transmit(&huart2, (uint8_t*)res, strlen(res), 200);
}
