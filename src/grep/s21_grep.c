#include "s21_grep.h"

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/code_errors.h"
#include "../common/file_utils.h"
#include "../common/safe_alloc.h"

// grep
//      -e - указание нескольких шаблонов
//      -i - игнорирование регистров шаблона
//      -v - инверсия совпадения
//      -c - подсчет свопадений
//      -l - названия файлов в которых есть свопадения
//      -n - номер строки совпадения
//      -h - не выводит имя файла перед строкой
//      -s - подавляет сообщения об ошибках о несуществующих или нечитаемых
//      файлах. -f - получает регулярные выражения из файла -o - вывод только
//      совпадений а не целых строк

static char *program_name = "s21_grep";

#define OPT_STRING "e:f:ivcloshn"
static struct option const long_options[] = {
    {"regexp", required_argument, NULL, 'e'},
    {"file", required_argument, NULL, 'f'},
    {"ignore-case", no_argument, NULL, 'i'},
    {"invert-match", no_argument, NULL, 'v'},
    {"count", no_argument, NULL, 'c'},
    {"files-with-matches", no_argument, NULL, 'l'},
    {"only-matching", no_argument, NULL, 'o'},
    {"no-message", no_argument, NULL, 's'},
    {"no-filename", no_argument, NULL, 'h'},
    {"line-number", no_argument, NULL, 'n'},
};

static opt_t opts = {
    .match_icase = false,         // i +
    .out_line = false,            // n +
    .only_matching = false,       // o
    .suppress_errors = false,     // s +
    .out_invert = false,          // v +
    .count_matches = false,       // c +
    .files_with_matches = false,  // l +
    .no_filename = -1,            // h +
};

int add_pattern_to_patterns(keys_t *patterns, const char *str) {
  int rc = EXIT_SUCCESS;

  if (patterns->data_size == patterns->alloc_size) {
    size_t new_alloc_size =
        (patterns->alloc_size == 0) ? 1 : patterns->alloc_size * 2;
    char **new_data = (char **)safe_alloc(patterns->data, new_alloc_size,
                                          sizeof(char *), &rc);
    if (!rc) {
      patterns->data = new_data;
      patterns->alloc_size = new_alloc_size;
    }
  }
  if (!rc) {
    patterns->data[patterns->data_size] =
        (char *)safe_alloc(NULL, strlen(str) + 1, sizeof(char), &rc);
  }
  if (!rc) {
    strcpy(patterns->data[patterns->data_size], str);
    patterns->data_size++;
  }
  return rc;
}

int read_file_to_patterns(keys_t *patterns, const char *filename) {
  int rc = EXIT_SUCCESS;

  char *line = NULL;
  size_t len = 0;
  ssize_t read = 0;

  FILE *file = fopen(filename, "r");
  if (!file) {
    rc = FOPEN_ERROR;
  }

  while (!rc && (read = getline(&line, &len, file)) != -1) {
    if (line[read - 1] == '\n') {
      line[read - 1] = '\0';
    }
    rc = add_pattern_to_patterns(patterns, line);
  }
  FREE(line);
  if (file) {
    fclose(file);
  }
  return rc;
}

int patterns_compile(keys_t src, comp_keys_t *dst) {
  int rc = EXIT_SUCCESS;
  size_t new_alloc_count = src.data_size;
  dst->data =
      (regex_t *)safe_alloc(dst->data, new_alloc_count, sizeof(regex_t), &rc);
  if (!rc) {
    dst->alloc_size = new_alloc_count * sizeof(regex_t);
    for (size_t i = 0; i < src.data_size; i++) {
      int re = regcomp(&(dst->data[dst->data_size]), src.data[i],
                       !opts.match_icase ? 0 : REG_ICASE);
      if (re == 0) {
        dst->data_size++;
      }
    }
    if (dst->data_size == 0) {
      rc = EMPTY_KEYS;
    }
  }

  return rc;
}

int compare_matches(const void *a, const void *b) {
  return ((regmatch_t *)a)->rm_so - ((regmatch_t *)b)->rm_so;
}

int is_match(const char *str, comp_keys_t compiled_patterns,
             regmatch_t *matches, int *match_count) {
  bool result = false;
  for (size_t i = 0; i < compiled_patterns.data_size; i++) {
    regex_t *regex = &compiled_patterns.data[i];
    regmatch_t match;

    int start = 0;
    while (!regexec(regex, str + start, 1, &match, 0)) {
      matches[*match_count].rm_so = match.rm_so + start;
      matches[*match_count].rm_eo = match.rm_eo + start;
      (*match_count)++;
      start = match.rm_eo + start;
      result = true;
    }
  }
  if (opts.out_invert) {
    result = !result;
  }
  return result;
}

void print_match(char *start, regmatch_t *matches, int match_count,
                 char *filename, int file_str) {
  qsort(matches, match_count, sizeof(regmatch_t), compare_matches);
  if (!opts.out_invert) {
    for (int i = 0; i < match_count; i++) {
      if (!opts.no_filename) {
        printf("%s:", filename);
      }
      if (opts.out_line) {
        printf("%d:", file_str);
      }
      printf("%.*s\n", (int)(matches[i].rm_eo - matches[i].rm_so),
             start + matches[i].rm_so);
    }
  }
}

void handle_match(char *start, regmatch_t *matches, int match_count,
                  char *filename, int file_str, int *result_match,
                  bool *file_match_exist) {
  if (opts.files_with_matches) {
    *file_match_exist = true;
  } else if (opts.count_matches) {
    (*result_match)++;
  } else if (opts.only_matching) {
    print_match(start, matches, match_count, filename, file_str);
  } else {
    if (!opts.no_filename) {
      printf("%s:", filename);
    }
    if (opts.out_line) {
      printf("%d:", file_str);
    }
    printf("%s\n", start);
  }
}

void process_line(char *start, comp_keys_t compiled_patterns, char *filename,
                  int file_str, int *result_match, bool *file_match_exist) {
  regmatch_t matches[MAX_MATCHES];
  int match_count = 0;
  if (is_match(start, compiled_patterns, matches, &match_count)) {
    handle_match(start, matches, match_count, filename, file_str, result_match,
                 file_match_exist);
  }
}

void grep_handle(char *buf, comp_keys_t compiled_patterns, char *filename) {
  char *start = buf;
  char *end = buf;
  int file_str = 0;
  int result_match = 0;
  bool file_match_exist = false;

  while (*end != '\0') {
    if (*end == '\n') {
      file_str++;
      *end = '\0';
      process_line(start, compiled_patterns, filename, file_str, &result_match,
                   &file_match_exist);
      end++;
      start = end;
    } else {
      end++;
    }
  }

  if (start != end) {
    file_str++;
    process_line(start, compiled_patterns, filename, file_str, &result_match,
                 &file_match_exist);
  }

  if (opts.files_with_matches) {
    if (file_match_exist) {
      printf("%s\n", filename);
    }
  } else if (opts.count_matches) {
    if (!opts.no_filename) {
      printf("%s:", filename);
    }
    printf("%d\n", result_match);
  }
}

int parse_options(int argc, char **argv, keys_t *patterns) {
  int rc = EXIT_SUCCESS;

  int opt;
  while (!rc && (opt = getopt_long(argc, argv, OPT_STRING, long_options,
                                   NULL)) != -1) {
    switch (opt) {
      case 'i':
        opts.match_icase = true;
        break;
      case 'n':
        opts.out_line = true;
        break;
      case 'o':
        opts.only_matching = true;
        break;
      case 's':
        opts.suppress_errors = true;
        break;
      case 'v':
        opts.out_invert = true;
        break;
      case 'c':
        opts.count_matches = true;
        break;
      case 'e':
        rc = add_pattern_to_patterns(patterns, optarg);
        break;
      case 'f':
        rc = read_file_to_patterns(patterns, optarg);
        if (rc == FOPEN_ERROR) {
          fprintf(stderr, "%s: No such file\n", optarg);
        }
        break;
      case 'h':
        opts.no_filename = 1;
        break;
      case 'l':
        opts.files_with_matches = true;
        break;
      case '?':
      default:
        rc = WRONG_ARG;
        usage();
    }
  }
  return rc;
}

int main(int argc, char **argv) {
  int exit_code = EXIT_SUCCESS;

  keys_t patterns = {NULL, 0, 0};
  comp_keys_t compiled_patterns = {NULL, 0, 0};

  exit_code = parse_options(argc, argv, &patterns);
  int argind = optind;
  char *inbuf = NULL;
  if (!exit_code && patterns.data == NULL) {
    if (argind >= argc) {
      exit_code = NO_REQ_ARG;
      usage(exit_code);
    } else {
      exit_code = add_pattern_to_patterns(&patterns, argv[argind]);
      argind++;
    }
  }
  if (!exit_code) {
    exit_code = patterns_compile(patterns, &compiled_patterns);
  }
  if (!exit_code) {
    if (argc - argind > 1 && opts.no_filename != 1) {
      opts.no_filename = 0;
    }
    while (argind < argc) {
      exit_code = read_file_to_buffer(&inbuf, argv[argind]);
      if (!exit_code) {
        grep_handle(inbuf, compiled_patterns, argv[argind]);
      } else if ((!opts.suppress_errors) && (exit_code = FOPEN_ERROR)) {
        fprintf(stderr, "%s: No such file\n", argv[argind]);
      }
      FREE(inbuf);
      argind++;
    }
  }
  if (patterns.data) {
    for (size_t i = 0; i < patterns.data_size; i++) {
      FREE(patterns.data[i]);
    }
    FREE(patterns.data);
  }

  if (compiled_patterns.data) {
    for (size_t i = 0; i < compiled_patterns.data_size; i++) {
      regfree(&compiled_patterns.data[i]);
    }
    FREE(compiled_patterns.data);
  }

  return exit_code;
}

void usage() {
  fprintf(stderr, "Usage: %s [OPTION]... PATTERNS [FILE]...\n", program_name);
  printf("Usage: %s [OPTION]... PATTERNS [FILE]...\n", program_name);
  printf("Search for PATTERNS in each FILE.\n");
  printf(
      "\
Example: %s -i 'hello world' menu.h main.c\n\
PATTERNS can contain multiple patterns separated by newlines.\n\
\n\
Pattern selection and interpretation:\n",
      program_name);
  printf(
      "\
    -e, --regexp=PATTERNS     use PATTERNS for matching\n\
    -f, --file=FILE           take PATTERNS from FILE\n\
    -i, --ignore-case         ignore case distinctions in patterns and data\n");
  printf(
      "\
\n\
Miscellaneous:\n\
    -s, --no-messages         suppress error messages\n\
    -v, --invert-match        select non-matching lines\n");
  printf(
      "\
\n\
Output control:\n\
    -n, --line-number         print line number with output lines\n\
    -h, --no-filename         suppress the file name prefix on output\n\
");
  printf(
      "\
    -o, --only-matching       show only nonempty parts of lines that match\n\
");
  printf(
      "\
    -l, --files-with-matches  print only names of FILEs with selected lines\n\
    -c, --count               print only a count of selected lines per FILE\n");
}
