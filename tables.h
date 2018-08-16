#ifndef INCLUDE_TABLES_H
#define INCLUDE_TABLES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GDT_NUM_ENTRIES 3

struct gdt_entry {
  uint16_t limit_low; /* The lower 16 bits of the limit */
  uint16_t base_low;  /* The lower 16 bits of the base */
  uint8_t base_mid;   /* The next 8 bits of the base */
  /* access byte
   * present: 1bit ; privilege: 2 bits; 1 bit empty/reserved;  */
  /* executable: 1bit; direction: 1bit; R/W: 1bit; Accessed: 1 bit;
   */
  uint8_t access;             /* Contains access flags */
  unsigned int limit_mid : 4; /* high 4 bits of limit */
  /* flags field
   * granularity: 1bit; size: 1bit; 2bits empty/reserved
   */
  unsigned int flags : 4;
  uint8_t base_high; /* The last 8 bits of the base; */

} __attribute__((packed)); /* It needs to be packed like this, 64 bits */

typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr {
  uint16_t limit; /* Size of gdt table in bytes*/
  uint32_t base;  /* Address to the first gdt entry */
} __attribute__((packed));

typedef struct gdt_ptr gdt_ptr_t;

void gdt_load_and_set(uint32_t);

void gdt_init(void);
#endif /* INCLUDE_TABLES_H */
