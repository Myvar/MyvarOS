#include <stdarg.h>
#include "main.h"
#include "heap.h"
#include "sedna.h"

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

// Global variable to hold kernel heap header.
KHEAPBM g_k_heap;

void *kmalloc(unsigned int size)
{
  void *tmp;

  // A quick sanity check to help with debugging.
  tmp = k_heapBMAlloc(&g_k_heap, size);
  if (tmp == 0)
  {
    panic("kmalloc OOM");
  }

  return tmp;
}

void kfree(void *ptr)
{
  k_heapBMFree(&g_k_heap, ptr);
}

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
    puts("exclude-start: ");
    itoa(split_by_off[x], 10, buf);
    puts(buf);
    puts(" exclude-size: ");
    itoa(split_by_size[x], 10, buf);
    puts(buf);
    puts("\n");
  }

  while ((unsigned int)mmap_entry < mbinfo->mmap_length + mbinfo->mmap_addr)
  {
    if (mmap_entry->type == 1)
    {
      // A type of 1 indicates that this block of memory is avaliable for usage and not
      // reserved by the hardware. Add this block to the heap for management.
      base = mmap_entry->base_addr_low;
      size = mmap_entry->length_low;

      puts("evaluating memory block; base: ");
      itoa(base, 10, buf);
      puts(buf);
      puts(" size: ");
      itoa(size, 10, buf);
      puts(buf);
      puts("\n");

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
        puts("adding memory block to heap; base: ");
        itoa(o_off[x], 10, buf);
        puts(buf);
        puts(" size: ");
        itoa(o_size[x], 10, buf);
        puts(buf);
        puts("\n");
        k_heapBMAddBlock(&g_k_heap, o_off[x], o_size[x], 16);
        total_added += o_size[x];
      }

      puts("ok\n");
    }

    mmap_entry = (MBMMAPENTRY *)((unsigned int)mmap_entry + mmap_entry->size + 4);
  }

  itoa(total_added, 10, buf);
  puts(buf);
  puts(" bytes added to kernel heap");
  puts("\n");
}

// This is used in the calculation to determine our actual load address.
extern unsigned char g_ImageRefCall;

void kernel_main(unsigned int boot_ref_addr)
{
  // Need to do some whacky offsets in order to grab actual location of `KERNEL_START_ADDR` in order
  // to store `boot_ref_addr` (once corrected) into it. This is used so we know where the kernel resides
  // in physical memory and we know where it resides in virtual memory.
  //
  // kernel physical memory location -      KERNEL_START_ADDR
  // kernel virtual memory location -       KERNEL_START_VADDR
  // kernel image size -                    KERNEL_END_VADDR - KERNEL_START_VADDR

  boot_ref_addr = boot_ref_addr - ((unsigned int)&g_ImageRefCall - (unsigned int)&KERNEL_START_VADDR);

  ((unsigned int *)(boot_ref_addr + ((unsigned int)&KERNEL_START_ADDR - (unsigned int)&KERNEL_START_VADDR)))[0] = boot_ref_addr;

  Init_Paging();
}

void panic(char *msg)
{
  puts("PANIC: ");
  puts(msg);
  puts("\n");
  for (;;)
    ;
}

const char *itoh_map = "0123456789ABCDEF";

char *itoh(int i, char *buf)
{
  int n;
  int b;
  int z;
  int s;

  if (sizeof(void *) == 4)
    s = 8;
  if (sizeof(void *) == 8)
    s = 16;

  for (z = 0, n = (s - 1); n > -1; --n)
  {
    b = (i >> (n * 4)) & 0xf;
    buf[z] = itoh_map[b];
    ++z;
  }
  buf[z] = 0;
  return buf;
}

void kprintf(const char *fmt, ...)
{
  const char *p;
  va_list argp;
  int i;
  char *s;
  char fmtbuf[256];

  va_start(argp, fmt);

  for (p = fmt; *p != '\0'; p++)
  {
    //kputc('w');
    if (*p != '%')
    {
      putc(*p);
      continue;
    }

    switch (*++p)
    {
    case 'c':
      i = va_arg(argp, int);
      putc(i);
      break;
    case 's':
      s = va_arg(argp, char *);
      puts(s);
      break;
    case 'x':
      i = va_arg(argp, int);
      s = itoh(i, fmtbuf);
      puts(s);
      break;
    case 'n':
      i = va_arg(argp, int);
      itoa(fmtbuf, 10, s);
      puts(s);
      break;
    case '%':
      putc('%');
      break;
    }
  }
  va_end(argp);
}

void kernel_HigherHalf()
{
  Serial_SetPort(0x3F8); //COM1
  Serial_Init();

  Termianl_Init();
  Termianl_Clear();

  putLog("Started Serial Terminal");

  Idt_Install();

  putLog("IDT Installed");

  Isrs_Install();

  putLog("Isrs Installed");

  Irq_Install();

  putLog("Irq Installed");

  putLog("Installing Memmory");
  init_memory_management();

  

  __asm__ __volatile__("sti");
  putLog("MyvarOS booted");

  putLog("Starting ide driver");
  ide_init(0x1F0);

  putLog("Initing Sedna");
  Sedna_Init();

  putLog("Installing Keybord");
  keyboard_install();

  //putLog("Starting Shell");
  //Shell_Init();

  while (1 == 1)
  {
    //Kb_Sendkey(Read_Serial());
    
    Write_Serial_Str("update~some_var!lol");
    putc(Read_Serial());
  }
}

char tbuf[32];
char bchars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void *memcpy(void *dest, const void *src, int count)
{
  const char *sp = (const char *)src;
  char *dp = (char *)dest;
  for (; count != 0; count--)
    *dp++ = *sp++;
  return dest;
}

void *memset(void *dest, char val, int count)
{
  char *temp = (char *)dest;
  for (; count != 0; count--)
    *temp++ = val;
  return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
  unsigned short *temp = (unsigned short *)dest;
  for (; count != 0; count--)
    *temp++ = val;
  return dest;
}

void itoa(unsigned i, unsigned base, char *buf)
{
  int pos = 0;
  int opos = 0;
  int top = 0;

  if (i == 0 || base > 16)
  {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }

  while (i != 0)
  {
    tbuf[pos] = bchars[i % base];
    pos++;
    i /= base;
  }
  top = pos--;
  for (opos = 0; opos < top; pos--, opos++)
  {
    buf[opos] = tbuf[pos];
  }
  buf[opos] = 0;
}

void itoa_s(int i, unsigned base, char *buf)
{
  if (base > 16)
    return;
  if (i < 0)
  {
    *buf++ = '-';
    i *= -1;
  }
  itoa(i, base, buf);
}