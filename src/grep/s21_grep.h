#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_MATCHES 1000

typedef struct options {
  bool match_icase;      // i
  bool out_line;         // n
  bool only_matching;    // o
  bool suppress_errors;  // s
  bool out_invert;       // v
  bool count_matches;    // c
  bool files_with_matches;
  int no_filename;  // h
} opt_t;

typedef struct keys {
  char **data;
  size_t alloc_size;
  size_t data_size;
} keys_t;

typedef struct compiled_keys {
  regex_t *data;
  size_t alloc_size;
  size_t data_size;
} comp_keys_t;

int add_pattern_to_patterns(keys_t *patterns, const char *str);
int read_file_to_patterns(keys_t *patterns, const char *filename);
int patterns_compile(keys_t src, comp_keys_t *dst);
int parse_options(int argc, char **argv, keys_t *patterns);
int compare_matches(const void *a, const void *b);
void usage();
int is_match(const char *str, comp_keys_t compiled_patterns,
             regmatch_t *matches, int *match_count);
void print_match(char *start, regmatch_t *matches, int match_count,
                 char *filename, int file_str);
void handle_match(char *start, regmatch_t *matches, int match_count,
                  char *filename, int file_str, int *result_match,
                  bool *file_match_exist);
void process_line(char *start, comp_keys_t compiled_patterns, char *filename,
                  int file_str, int *result_match, bool *file_match_exist);
void grep_handle(char *buf, comp_keys_t compiled_patterns, char *filename);

#endif  // S21_GREP_H