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

void clear_buffer(char* buffer) {
	memset(buffer, '\0', DEFAULT_BUFFER_SIZE);
}
