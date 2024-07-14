#ifndef S21_CAT
#define S21_CAT

#include <stdbool.h>
#include <stddef.h>
#define IO_BUFSIZE 256 * 1024

typedef struct options {
  bool show_nonprinting;  // v
  bool show_tabs;         // T
  bool show_ends;         // E
  bool number;            // n
  bool number_nonblank;   // b
  bool squeeze_blank;     // s
} opt_t;

void handle_nonprinting(unsigned char ch, char** bpout);
size_t cat(char* inbuf, size_t size_file, char** outbuf);
int parse_options(int argc, char** argv);
void usage();
#endif  // S21_CAT