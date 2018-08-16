#ifndef INCLUDE_STR_H
#define INCLUDE_STR_H

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *str);
size_t format_param_count(const char *str);
void format_string(char *output, const char *input, uint8_t *vals);

#endif /* INCLUDE_STR_H */
