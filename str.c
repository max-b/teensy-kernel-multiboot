#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "str.h"


/** strlen
 *  Returns the length of a null terminated string
 *
 *  @param str A pointer to a null terminated string
 */
size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len]) {
    len++;
  }
  return len;
}

size_t format_param_count(const char* str) {
  size_t num = 0;
  size_t i;

  for (i = 0; i < strlen(str); i++) {
    if (str[i] == '%' && str[i+1] == '%') {
      num += 1;
      i += 1;
    }
  }

  return num;
}

uint8_t hex_representation(unsigned short nibble) {
  uint8_t output;

  output = ((nibble & 0x0f) + 0x30);

  if (output > 0x39) {
    output += 0x27;
  }

  return output;
}

/** format_string
 *  Takes a format string and a list of vals and
 *  puts the properly interpolated values into the
 *  output string.
 *  '%%' will signify the byte to be interpolated
 *  Currently vals can only be bytes represented by uint8_t
 *  TODO: Return an error code?
 *  TODO: Handle other types
 *
 *  @param output A pointer to the string to write formatted string to
 *  @param input A null terminated string
 *  @param vals A pointer to an array of bytes to interpolate
 */
void format_string(char *output, const char *input, uint8_t *vals) {
  size_t i;
  size_t val_index = 0;

  for (i = 0; i < strlen(input); i++) {
    if (input[i] == '%' && input[i+1] == '%') {
      output[i] = (char) hex_representation(vals[val_index] >> 4);
      output[i+1] = (char) hex_representation(vals[val_index]);

      val_index += 1;
      i += 1;
    } else {
      output[i] = input[i];
    }
  }

  output[i] = 0x00;
}
