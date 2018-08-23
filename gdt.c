#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "io.h"
#include "gdt.h"

gdt_entry_t gdt_entries[GDT_NUM_ENTRIES];

gdt_entry_t null_entry = {};

/*
 * Code entry
 * base: 0x00;
 * limit: 0xfffff
 * (actual limit will be 0xffffff because granularity bit set to 1, so page size
 * 4KiB);
 * access: 0x92
 * 1 (present) 00 (privilege) 1 (empty/reserved) 1 (executable)  0 (conforming)
 * 1 (readable) 0 (accessed)
 * flags: 0xc
 * 1 (granularity) 1 (size) 00 (empty/reserved)
 */
gdt_entry_t code_entry = {
    .limit_low = 0xffff,
    .base_low = 0x0000,
    .base_mid = 0x00,
    .access = 0x9a,
    .limit_mid = 0xf,
    .flags = 0xc,
    .base_high = 0x00,
};

/*
 * Data entry
 * base: 0x00;
 * limit: 0xfffff
 * (actual limit will be 0xffffff because granularity bit set to 1, so page size
 * 4KiB);
 * access: 0x92
 * 1 (present) 00 (privilege) 1 (empty/reserved) 0 (executable)  0 (direction -
 * segment grows up) 1 (writeable) 0 (accessed)
 * flags: 0xc
 * 1 (granularity) 1 (size) 00 (empty/reserved)
 */
gdt_entry_t data_entry = {
    .limit_low = 0xffff,
    .base_low = 0x0000,
    .base_mid = 0x00,
    .access = 0x92,
    .limit_mid = 0xf,
    .flags = 0xc,
    .base_high = 0x00,
};

// TODO: Apparently I need a TSS, but first I need to find out what a TSS is?

void gdt_init(void) {
  gdt_ptr_t gdt_ptr;
  gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_NUM_ENTRIES;
  gdt_ptr.base = (uint32_t)&gdt_entries;

  gdt_entries[0] = null_entry;
  gdt_entries[1] = code_entry; /* selector 0x08 */
  gdt_entries[2] = data_entry; /* selector  0x10 */

  gdt_load_and_set((uint32_t)&gdt_ptr);
}
