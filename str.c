#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "str.h"


/** strlen
 *  Returns the length of a null terminated string
 *
 *  @param str A pointer to a null terminated string
 */
size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
