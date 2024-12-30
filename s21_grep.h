#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdio.h>

#define REG_CASE REG_ICASE
#define REG_EXT REG_EXTENDED

#define DIRECTORY "Is a directory"
#define OTHER "Other type of file"
#define PATH "No such file or directory"
#define DENIED "Permission denied"
#define NO_PATTERN "no pattern"
#define FLAG_ARGUMENT "option requires an argument --"

#define MAX_PATTERN 100
#define INITIAL_CAPACITY 2

typedef struct {
  int no_argument;
  int path_error;
  int ef_error;
  int ef_pattern;
  int flag;
  int flag_error;
  int file;
  int file_number;
  int file_pattern;
  int counter;
  int true_count;
  int false_count;
  int breaker;
  int same;
  int pattern;
  int pattern_number;
  int pattern_capacity;
  int exit;
  int macOS;
} Check;

typedef struct {
  int e;
  int f;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int o;
} Flag;

void parse_command_line_arguments(Check *check, Flag *flag, char *argv[],
                                  char ***pattern_array, char *file_array[],
                                  int argc);

void parse_flags(Check *check, Flag *flag, char ***pattern_array, char *argv[],
                 const char *argument, int argc, int *arg_number);

void parse_file_or_pattern(Check *check, const Flag *flag,
                           char *pattern_array[], char *file_array[],
                           char *argv[], char *argument, int argc);

void work_with_pattern(Check *check, Flag *flags, char ***pattern_array,
                       const char *argument, char *next_argument,
                       int *loop_number);

void initial_memory(Check *check, char ***array);

void adding_pattern_to_array(Check *check, char ***array, const char *pattern);

void free_dynamic_memory(const Check *check, char **array);

void change_flag_status(Check *check, Flag *flags, const char *flag);

void detect_macOS(Check *check);

void check_path(Check *check, const Flag *flag, char ***patterns, char *path);

void file_input(Check *check, const Flag *flag, char ***patterns, char *path);

void terminal_input(Check *check, const Flag *flag, char *arguments[]);

void handle_input_lines(Check *check, const Flag *flag, FILE *file,
                        char ***array, char *path);

int display_file_match_count(Check *check, const Flag *flag, char *array[],
                             const char *path, const char *file_pattern,
                             char *line);

void shows_count(Check *check, const Flag *flag, const char *path);

void process_after_loop(Check *check, const Flag *flag, char *file_array[],
                        char *pattern_array[]);

int find_pattern_in_line(const Check *check, const Flag *flag, char **array,
                         const char *path, const char *file_pattern,
                         char *line);

void output_exact_match(const Check *check, const Flag *flag,
                        const regmatch_t *match, const char *path,
                        const char *file_pattern, int *start, int exactMatch,
                        const char *line);

void output_match(const Check *check, const Flag *flag, char *array[],
                  const char *path, const char *file_pattern, char *line);

void output_dontmatch(const Check *check, const Flag *flag, char *array[],
                      const char *path, const char *file_pattern, char *line);

void add_filepath_prefix(const Check *check, const Flag *flag,
                         const char *path);

void change_with_n(const Check *check, const Flag *flag);

void change_with_c(Check *check, const Flag *flag, const char *path);

int check_pattern_for_ef(int argc, const char *argv[]);

void check_same_path(Check *check, const char *path, const char *path_check);

void check_and_append_newline(char *line, ssize_t *read);

void activate_flag(Check *check, int *flag);

void activate_flags(Check *check, int *flag);

void counters_to_zero(Check *check);

void counts_match(Check *check, const Flag *flag, char *array[],
                  const char *path, const char *file_pattern, char *line);

int vlco_flags_zero(const Flag *flag);

int vlc_flags_zero(const Flag *flag);

void error_message(const Check *status, const char *path, const char *type);

#endif
