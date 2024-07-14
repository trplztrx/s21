#include "s21_cat.h"

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/code_errors.h"
#include "../common/file_utils.h"
#include "../common/safe_alloc.h"
#include "line_counter.h"

static char* program_name = "s21_cat";

#define OPT_STRING "benstETv"
struct option const long_options[] = {
    {"number-nonblank", no_argument, NULL, 'b'},  // нумерует непустые строки
    {"number", no_argument, NULL, 'n'},  // нумерует все выходные строки
    {"squeeze-blank", no_argument, NULL,
     's'},  // сжимает несколько смежных пустых строк
    {"show-nonprinting", no_argument, NULL,
     'v'},  // вывод непечатающихся(скрытых) символов
};

static opt_t opts = {.show_nonprinting = false,
                     .show_tabs = false,
                     .show_ends = false,
                     .number = false,
                     .number_nonblank = false,
                     .squeeze_blank = false};

void handle_nonprinting(unsigned char ch, char** bpout) {
  if (ch < 32) {
    *(*bpout)++ = '^';
    *(*bpout)++ = ch + 64;
  } else if (ch == 127) {
    *(*bpout)++ = '^';
    *(*bpout)++ = '?';
  } else if (ch >= 128) {
    *(*bpout)++ = 'M';
    *(*bpout)++ = '-';
    if (ch < 128 + 32) {
      *(*bpout)++ = '^';
      *(*bpout)++ = ch - 128 + 64;
    } else if (ch < 128 + 127) {
      *(*bpout)++ = ch - 128;
    } else {
      *(*bpout)++ = '^';
      *(*bpout)++ = '?';
    }
  } else {
    *(*bpout)++ = ch;
  }
}

static bool newline = true;
static bool squeeze = false;

size_t cat(char* inbuf, size_t size_file, char** outbuf) {
  char* bpin = inbuf;
  char* bpout = *outbuf;
  while (bpin < inbuf + size_file) {
    unsigned char ch = *bpin++;
    if (newline) {
      if (opts.squeeze_blank && ch == '\n') {
        if (squeeze) continue;
        squeeze = true;
      } else {
        squeeze = false;
        newline = false;
      }
      if (opts.number && !(opts.number_nonblank && ch == '\n')) {
        next_line_counter();
        bpout = strcpy(bpout, line_counter_min_print);
        while (*bpout) bpout++;
        newline = false;
      }
    }
    if (opts.show_nonprinting) {
      if (ch == '\n') {
        if (opts.show_ends) *bpout++ = '$';
        newline = true;
        *bpout++ = ch;
      } else if (ch == '\t' && !opts.show_tabs) {
        *bpout++ = '\t';
      } else {
        handle_nonprinting(ch, &bpout);
      }
    } else {
      if (ch == '\t' && opts.show_tabs) {
        *bpout++ = '^';
        *bpout++ = ch + 64;
      } else if (ch == '\n') {
        if (opts.show_ends) *bpout++ = '$';
        newline = true;
        *bpout++ = ch;
      } else {
        *bpout++ = ch;
      }
    }
  }
  *bpout = '\0';

  return bpout - *outbuf;
}

int parse_options(int argc, char** argv) {
  int rc = EXIT_SUCCESS;

  int c;
  while ((c = getopt_long(argc, argv, OPT_STRING, long_options, NULL)) != -1) {
    switch (c) {
      case 'b':
        opts.number = true;
        opts.number_nonblank = true;
        break;
      case 'e':
        opts.show_ends = true;
        opts.show_nonprinting = true;
        break;
      case 'n':
        opts.number = true;
        break;
      case 's':
        opts.squeeze_blank = true;
        break;
      case 't':
        opts.show_tabs = true;
        opts.show_nonprinting = true;
        break;
      case 'T':
        opts.show_tabs = true;
        break;
      case 'E':
        opts.show_ends = true;
        break;
      case 'v':
        opts.show_nonprinting = true;
        break;
      default:
        rc = WRONG_ARG;
        usage();
    }
  }

  return rc;
}

int main(int argc, char** argv) {
  int exit_code = EXIT_SUCCESS;

  exit_code = parse_options(argc, argv);

  int argind = optind;
  char* inbuf = NULL;
  char* outbuf = NULL;
  while (!exit_code && argind < argc) {
    long size_infile = 0;

    char* infile_name = argv[argind];

    exit_code = read_file_to_buffer(&inbuf, infile_name);
    if (!exit_code) exit_code = get_size_file(&size_infile, infile_name);
    if (!exit_code) {
      size_t size_result = 0;
      outbuf = (char*)malloc(size_infile * 4 + LINE_COUNTER_BUF_LEN - 1 +
                             IO_BUFSIZE);

      size_result = cat(inbuf, size_infile, &outbuf);

      for (size_t i = 0; i < size_result; i++) {
        printf("%c", outbuf[i]);
      }
    }

    FREE(inbuf);
    FREE(outbuf);

    argind++;
  }

  return exit_code;
}

void usage() {
  printf(
      "\
Usage: %s [OPTION]... [FILE]...\n\
",
      program_name);
  fputs(
      "\
Concatenate FILE(s) to standard output.\n\
",
      stdout);
  fputs(
      "\
\n\
  -b, --number-nonblank    number nonempty output lines, overrides -n\n\
  -e                       equivalent to -vE\n\
  -E, --show-ends          display $ at end of each line\n\
  -n, --number             number all output lines\n\
  -s, --squeeze-blank      suppress repeated empty output lines\n\
",
      stdout);
  fputs(
      "\
  -t                       equivalent to -vT\n\
  -T, --show-tabs          display TAB characters as ^I\n\
  -v, --show-nonprinting   use ^ and M- notation, except for LFD and TAB\n\
",
      stdout);
}