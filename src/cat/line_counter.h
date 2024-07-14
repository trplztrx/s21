#ifndef LINE_COUNTER_H
#define LINE_COUNTER_H

#define LINE_COUNTER_BUF_LEN 20   // GNU number
#define MIN_LINE_COUNTER_PRINT 8  // GNU number

extern char line_counter_buf[LINE_COUNTER_BUF_LEN];
extern char* line_counter_min_print;
extern char* line_counter_start;
extern char* line_counter_end;

void next_line_counter();

#endif  // LINE_COUNTER_H