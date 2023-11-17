#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#define TERMINAL_LINE_WIDTH 11
#define TERMINAL_LINE_HEIGHT 18
#define DEFAULT_BUFFER_SIZE 100

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void format_buffer(char* buffer, size_t line_width);
void clear_buffer(char* buffer);


#endif /* INC_UTILS_H_ */
