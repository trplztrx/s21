CC 				:= gcc
STANDART 		:= -std=c11
CFLAGS 			:= -Wall -Werror -Wextra
POSIX_FLAG 		:= -D_POSIX_C_SOURCE=201710L
COM_OBJS_DIR 	:= ../common
CAT_DIR 		:= .

# EXTRA_FLAGS		:= -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable
OBJS			:= $(COM_OBJS_DIR)/safe_alloc.o $(COM_OBJS_DIR)/file_utils.o $(CAT_DIR)/line_counter.o

CAT_EXE 		:= $(CAT_DIR)/s21_cat

all: s21_cat

s21_cat: $(CAT_EXE)

$(CAT_EXE): $(CAT_DIR)/main.o $(OBJS)
	$(CC) $(STANDART) $(POSIX_FLAG) $(CFLAGS) $^ -o $@

$(CAT_DIR)/main.o: $(CAT_DIR)/s21_cat.c
	$(CC) -c $(STANDART) $(POSIX_FLAG) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(STANDART) $(POSIX_FLAG) $(CFLAGS) $< -o $@ 

.PHONY: clean rebuild test

clean:
	$(RM) $(CAT_DIR)/*.o $(CAT_EXE) $(OBJS)

rebuild: clean all

test: all 
	$(BASH) ./tests_cat.sh