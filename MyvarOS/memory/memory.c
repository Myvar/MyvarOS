#include "heap.h"
#include "tty.h"

extern unsigned int MultibootInfo_Structure;
extern unsigned int Inte801Ax;
extern unsigned int Inte801Dx;

// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
struct _MBMMAPENTRY
{
  unsigned int size;
  unsigned int base_addr_low;
  unsigned int base_addr_high;
  unsigned int length_low;
  unsigned int length_high;
  unsigned int type;
};

typedef struct _MBMMAPENTRY MBMMAPENTRY;

// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
struct _MBINFO
{
  unsigned int flags;               // 0
  unsigned int mem_lower;           // 4
  unsigned int mem_upper;           // 8
  unsigned int boot_device;         // 12
  unsigned int cmdline;             // 16
  unsigned int mods_count;          // 20
  unsigned int mods_addr;           // 24
  unsigned char syms[16];           // 28
  unsigned int mmap_length;         // 44
  unsigned int mmap_addr;           // 48
  unsigned int drives_length;       // 52
  unsigned int drives_addr;         // 56
  unsigned int config_table;        // 60
  unsigned int boot_loader_name;    // 64
  unsigned int apm_table;           // 68
  unsigned int vbe_control_info;    // 72
  unsigned int vbe_mode_info;       // 76
  unsigned short vbe_mode;          // 80
  unsigned short vbe_interface_seg; // 82
  unsigned short vbe_interface_off; // 84
  unsigned short vbe_interface_len; // 86
};

typedef struct _MBINFO MBINFO;

int split_range_by_range(unsigned int off, unsigned int size, unsigned int s_off, unsigned int s_size, unsigned int *o_off, unsigned int *o_size)
{
  unsigned int tmp;
  // Completely above (s_off:s_off+s_size).
  if (off >= s_off + s_size)
  {
    o_off[0] = off;
    o_off[1] = 0;
    o_size[0] = size;
    o_size[1] = 0;
    return 1;
  }

  // Completely below (s_off:s_off+s_size).
  if (off + size < s_off)
  {
    o_off[0] = off;
    o_off[1] = 0;
    o_size[0] = size;
    o_size[1] = 0;
    return 1;
  }

  // The start `off` is inside (s_off:s_off+s_size).
  if (off >= s_off)
  {
    tmp = s_size - (off - s_off);
    off += tmp;

    // Nothing left.
    if (tmp > size || size - tmp == 0)
    {
      o_off[0] = 0;
      o_off[1] = 0;
      o_size[0] = 0;
      o_size[1] = 0;
      return 0;
    }

    o_off[0] = off;
    o_off[1] = 0;
    o_size[0] = size - tmp;
    o_size[1] = 0;
    return 1;
  }

  // The end is inside (s_off:s_off+s_size).
  if (off < s_off && off + size >= s_off)
  {
    tmp = (off + size);

    if (off + size <= s_off + s_size)
    {
      tmp = (off + size) - s_off;
      size -= tmp;
      o_off[0] = off;
      o_off[1] = 0;
      o_size[0] = size;
      o_size[1] = 0;
      return 1;
    }

    o_off[0] = off;
    o_off[1] = s_off + s_size;
    o_size[0] = s_off - off;
    o_size[1] = (off + size) - (s_off + s_size);
    return 2;
  }

  return -1;
}

/*
  Take one range and split it by one or more ranges. The number of output ranges is returned and the results
  are stored in the `o_off` and `o_size` arrays. The `o_off` and `o_size` arrays must be pre-allocated and
  must be twice the elements as the number of split element pairs.
*/
int split_range_by_ranges(unsigned int off, unsigned int size, unsigned int *s_off, unsigned int *s_size, unsigned int s_cnt, unsigned int *o_off, unsigned int *o_size)
{
  unsigned int cnt;
  unsigned int new_cnt;
  unsigned int x;
  unsigned int y;
  unsigned int rc;
  unsigned int tmp_off[2];
  unsigned int tmp_size[2];

  o_off[0] = off;
  o_size[0] = size;

  cnt = 1;

  for (x = 0; x < s_cnt; ++x)
  {
    //printf("x: %u\n", x);
    new_cnt = cnt;
    for (y = 0; y < cnt; ++y)
    {
      //printf("cnt: %u off: %u size: %u s_off: %u s_size: %u new_cnt: %u\n", cnt, o_off[y], o_size[y], s_off[x], s_size[x], new_cnt);
      rc = split_range_by_range(o_off[y], o_size[y], s_off[x], s_size[x], tmp_off, tmp_size);

      //printf("rc: %u y:%u\n", rc, y);

      if (rc == 0)
      {
        o_off[y] = 0;
        o_size[y] = 0;
        new_cnt--;
      }
      else
      {
        if (rc > 0)
        {
          o_off[y] = tmp_off[0];
          o_size[y] = tmp_size[0];
        }

        if (rc > 1)
        {
          o_off[new_cnt] = tmp_off[1];
          o_size[new_cnt] = tmp_size[1];
          new_cnt++;
        }
      }
    }
    cnt = new_cnt;
  }

  return cnt;
}

// Global variable to hold kernel heap header.
KHEAPBM g_k_heap;

void *kmalloc(unsigned int size)
{
  void *tmp;

  // A quick sanity check to help with debugging.
  tmp = k_heapBMAlloc(&g_k_heap, size);
  if (tmp == 0)
  {
    tty_error("kmalloc OOM");
  }

  return tmp;
}

void kfree(void *ptr)
{
  k_heapBMFree(&g_k_heap, ptr);
}

// Defined in linker script.
extern unsigned char KERNEL_START_VADDR;
// Defined in linker script.
extern unsigned char KERNEL_END_VADDR;
// Set on boot by kernel entry point.
unsigned int KERNEL_START_ADDR;

void init_memory_management()
{
  // Used for formatting.
  char buf[255];
  unsigned int base;
  unsigned int size;
  unsigned int part_base[4];
  unsigned int part_size[4];
  unsigned int kernel_image_size = (unsigned int)&KERNEL_END_VADDR - (unsigned int)&KERNEL_START_VADDR;
  int part_cnt[2];
  unsigned int total_added = 0;

  unsigned int split_by_off[] = {
      (unsigned int)&KERNEL_START_VADDR, // kernel virtual address range offset
      KERNEL_START_ADDR,                 // kernel physical address range offset
      0,                                 // idt
  };
  unsigned int split_by_size[] = {kernel_image_size, kernel_image_size, 1024 * 1024};

  unsigned int o_off[sizeof(split_by_off) / sizeof(unsigned int) * 2];
  unsigned int o_size[sizeof(split_by_off) / sizeof(unsigned int) * 2];
  unsigned int split_cnt;

  // Examine the multi-boot structure. -kmcg
  // https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
  MBINFO *mbinfo = (MBINFO *)MultibootInfo_Structure;

  MBMMAPENTRY *mmap_entry = mbinfo->mmap_addr;

  k_heapBMInit(&g_k_heap);

  for (int x = 0; x < 3; ++x)
  {
    /*tty_puts("exclude-start: ");
    itoa(split_by_off[x], 10, buf);
    tty_puts(buf);
    tty_puts(" exclude-size: ");
    itoa(split_by_size[x], 10, buf);
    tty_puts(buf);
    tty_puts("\n");*/
  }

  while ((unsigned int)mmap_entry < mbinfo->mmap_length + mbinfo->mmap_addr)
  {
    if (mmap_entry->type == 1)
    {
      // A type of 1 indicates that this block of memory is avaliable for usage and not
      // reserved by the hardware. Add this block to the heap for management.
      base = mmap_entry->base_addr_low;
      size = mmap_entry->length_low;

      /*tty_puts("evaluating memory block; base: ");
      itoa(base, 10, buf);
      tty_puts(buf);
      puts(" size: ");
      itoa(size, 10, buf);
      tty_puts(buf);
      tty_puts("\n");*/

      // Take the range and exclude any ranges (split).
      split_cnt = split_range_by_ranges(base, size, &split_by_off[0], &split_by_size[0], 3, &o_off[0], &o_size[0]);

      // Iterate over the ranges left. It may be one (original) or up to two times the number of split ranges.
      for (int x = 0; x < split_cnt; ++x)
      {
        // Fix problem with split returning empty ranges.
        if (o_size[x] == 0)
        {
          continue;
        }
        /* puts("adding memory block to heap; base: ");
        itoa(o_off[x], 10, buf);
        puts(buf);
        puts(" size: ");
        itoa(o_size[x], 10, buf);
        puts(buf);
        puts("\n"); */
        k_heapBMAddBlock(&g_k_heap, o_off[x], o_size[x], 16);
       
        total_added += o_size[x];
      }

      //puts("ok\n");
    }

    mmap_entry = (MBMMAPENTRY *)((unsigned int)mmap_entry + mmap_entry->size + 4);
  }
  tty_set_color(0xF, 0x0);
  tty_putc('[');
  tty_set_color(0xA, 0x0);
  tty_puts("Log");
  tty_set_color(0xF, 0x0);
  tty_putc(']');
  itoa(total_added / 1048576, 10, buf);
  tty_puts(buf);
  tty_puts(" mb added to kernel heap");
  tty_putc('\n');
}