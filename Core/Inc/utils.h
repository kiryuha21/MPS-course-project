#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#define TERMINAL_LINE_WIDTH 11
#define TERMINAL_LINE_HEIGHT 18
#define DEFAULT_BUFFER_SIZE 100

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>

void format_buffer(char* buffer, size_t line_width);
void clear_buffer(char* buffer);
void char_array_to_uint32_array(char* src, uint32_t* dest, int len);
void uint32_array_to_char_array(uint32_t* src, char* dest, int len);
void to_lower(char* string);

#endif /* INC_UTILS_H_ */
