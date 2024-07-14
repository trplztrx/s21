CC 				:= gcc
STANDART 		:= -std=c11
CFLAGS 			:= -Wall -Werror -Wextra
POSIX_FLAG		:= -D_POSIX_C_SOURCE=201710L

GREP_DIR 		:= .
COM_OBJS_DIR	:= ../common

OBJS 			:= $(COM_OBJS_DIR)/safe_alloc.o $(COM_OBJS_DIR)/file_utils.o

GREP_EXE 		:= $(GREP_DIR)/s21_grep

all: s21_grep

s21_grep: $(GREP_EXE)

$(GREP_EXE): $(GREP_DIR)/main.o $(OBJS)
	$(CC) $(STANDART) $(POSIX_FLAG) $(CFLAGS) $^ -o $@

$(GREP_DIR)/main.o: $(GREP_DIR)/s21_grep.c 
	$(CC) -c $(STANDART) $(POSIX_FLAG) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(STANDART) $(POSIX_FLAG) $(CFLAGS) $< -o $@

.PHONY: clean rebuild test

clean:
	$(RM) $(GREP_DIR)/*.o $(GREP_EXE) $(OBJS) $(GREP_DIR)/*.txt

rebuild: clean all

test: all
	$(BASH) ./tests_grep.sh