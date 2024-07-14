#include "line_counter.h"

char line_counter_buf[LINE_COUNTER_BUF_LEN] = {
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', '0', '\t', '\0'};

char* line_counter_min_print =
    line_counter_buf + LINE_COUNTER_BUF_LEN - MIN_LINE_COUNTER_PRINT;
char* line_counter_start = line_counter_buf + LINE_COUNTER_BUF_LEN - 3;
char* line_counter_end = line_counter_buf + LINE_COUNTER_BUF_LEN - 3;

void next_line_counter() {
  char* endp = line_counter_end;
  do {
    if ((*endp)++ < '9') return;
    *endp-- = '0';
  } while (endp >= line_counter_start);

  if (line_counter_start > line_counter_buf)
    *--line_counter_start = '1';
  else
    *line_counter_buf = '>';

  if (line_counter_start < line_counter_min_print) line_counter_min_print--;
}
