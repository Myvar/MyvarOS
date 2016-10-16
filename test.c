#include <stdio.h>

int split_range_by_range(unsigned int off, unsigned int size, unsigned int s_off, unsigned int s_size, unsigned int *o_off, unsigned int *o_size) {
    unsigned int tmp;
    // Completely above (s_off:s_off+s_size).
    if (off >= s_off + s_size) {
      o_off[0] = off;
      o_off[1] = 0;
      o_size[0] = size;
      o_size[1] = 0;
      return 1; 
    }

    // Completely below (s_off:s_off+s_size).
    if (off + size < s_off) {
      o_off[0] = off;
      o_off[1] = 0;
      o_size[0] = size;
      o_size[1] = 0;
      return 1;       
    }

    // The start `off` is inside (s_off:s_off+s_size).
    if (off >= s_off) {
      tmp = s_size - (off - s_off);
      off += tmp;

      // Nothing left.
      if (tmp > size || size - tmp == 0) {
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
    if (off < s_off && off + size >= s_off) {
      tmp = (off + size);

      if (off + size <= s_off + s_size) {
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

int split_range_by_ranges(unsigned int off, unsigned int size, unsigned int *s_off, unsigned int *s_size, unsigned int s_cnt, unsigned int *o_off, unsigned int *o_size) {
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

    for (x = 0; x < s_cnt; ++x) {
      printf("x: %u\n", x);
      new_cnt = cnt;
      for (y = 0; y < cnt; ++y) {
        printf("cnt: %u off: %u size: %u s_off: %u s_size: %u new_cnt: %u\n", cnt, o_off[y], o_size[y], s_off[x], s_size[x], new_cnt);
        rc = split_range_by_range(o_off[y], o_size[y], s_off[x], s_size[x], tmp_off, tmp_size);

        printf("rc: %u y:%u\n", rc, y);

        if (rc == 0) {
          o_off[y] = 0;
          o_size[y] = 0;
          new_cnt--;
        } else {
          if (rc > 0) {
            o_off[y] = tmp_off[0];
            o_size[y] = tmp_size[0];
          }

          if (rc > 1) {
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

int main(int argc, char *argv[]) {
  unsigned int off[6];
  unsigned int size[6];

  unsigned int rng_off[] = { 1024*1024, 900000, 0  };
  unsigned int rng_size[] = { 1024*1024*4, 100000, 700000 };

  memset(off, 0, sizeof(off));
  memset(size, 0, sizeof(size));

  split_range_by_ranges(0, 650000, rng_off, rng_size, 3, off, size);

  printf("[0]: %u %u\n", off[0], size[0]);
  printf("[1]: %u %u\n", off[1], size[1]);
  printf("[2]: %u %u\n", off[2], size[2]);
  printf("[3]: %u %u\n", off[3], size[3]);
  printf("[4]: %u %u\n", off[4], size[4]);
  printf("[5]: %u %u\n", off[5], size[5]);
  return 0;
}