

#define _GNU_SOURCE
#include "s21_grep.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  Flag flag = {0};
  Check check = {0};

  char **pattern_array = NULL;
  char *file_array[MAX_PATTERN];
  if (argc == 1 || (argc == 2 && argv[1][0] == '-')) {
    check.no_argument = 1;
    error_message(&check, argv[0], NO_PATTERN);
  } else {
    parse_command_line_arguments(&check, &flag, argv, &pattern_array,
                                 file_array, argc);
  }
  return 0;
}

// With this function we parse agruments for identify what it is
void parse_command_line_arguments(Check *check, Flag *flag, char *argv[],
                                  char ***pattern_array, char *file_array[],
                                  int argc) {
  initial_memory(check, pattern_array);
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      parse_flags(check, flag, pattern_array, argv, argv[i], argc, &i);
    } else {
      parse_file_or_pattern(check, flag, *pattern_array, file_array, argv,
                            argv[i], argc);
    }
  }
  process_after_loop(check, flag, file_array, *pattern_array);

  free_dynamic_memory(check, *pattern_array);
}

// With this function we works with e and f flags
void parse_flags(Check *check, Flag *flag, char ***pattern_array, char *argv[],
                 const char *argument, int argc, int *arg_number) {
  if ((argument[1] == 'e' || argument[1] == 'f')) {
    if (*arg_number + 1 < argc) {
      if (argument[1] == 'e') {
        work_with_pattern(check, flag, pattern_array, argument,
                          argv[*arg_number + 1], arg_number);
      }
      if (argument[1] == 'f') {
        work_with_pattern(check, flag, pattern_array, argument,
                          argv[*arg_number + 1], arg_number);
      }
    } else {
      check->flag_error = 1;
      error_message(check, argument, FLAG_ARGUMENT);
    }
  } else if (strlen(argument) > 1) {
    change_flag_status(check, flag, argument);
  }
}

// With this function we works with files and not flags
void parse_file_or_pattern(Check *check, const Flag *flag,
                           char *pattern_array[], char *file_array[],
                           char *argv[], char *argument, int argc) {
  if (check_pattern_for_ef(argc, (const char **)argv) == 0 &&
      check->pattern == 0) {
    check->pattern = 1;
    adding_pattern_to_array(check, &pattern_array, argument);
    if (argc == 2) terminal_input(check, flag, pattern_array);
  } else {
    check->file = 1;
    if (check->file_number < MAX_PATTERN) {
      file_array[check->file_number++] = argument;
    }
  }
}

// With this function when there is e and f flags we works with patterns
void work_with_pattern(Check *check, Flag *flags, char ***pattern_array,
                       const char *argument, char *next_argument,
                       int *loop_number) {
  if (argument[1] == 'e') {
    activate_flags(check, &flags->e);
    adding_pattern_to_array(check, pattern_array, next_argument);
  }
  if (argument[1] == 'f') {
    activate_flags(check, &flags->f);
    check_path(check, flags, pattern_array, next_argument);
  }
  (*loop_number)++;
  check->ef_pattern = 0;
}

// With this function we allocate initial dynamic memory
void initial_memory(Check *check, char ***array) {
  check->pattern_capacity = INITIAL_CAPACITY;
  *array = malloc(check->pattern_capacity * sizeof(char *));
  if (*array == NULL) {
    exit(EXIT_FAILURE);
  }
}

// With this function we realloc dynamic memory and add patterns to array
void adding_pattern_to_array(Check *check, char ***array, const char *pattern) {
  if (check->pattern_number >= check->pattern_capacity) {
    check->pattern_capacity = check->pattern_capacity * 2;
    char **temp_array =
        realloc(*array, check->pattern_capacity * sizeof(char *));
    if (temp_array != NULL) {
      *array = temp_array;
    }
  }
  (*array)[check->pattern_number] = strdup(pattern);
  if ((*array)[check->pattern_number] != NULL) {
    (*array)[check->pattern_number]
            [strcspn((*array)[check->pattern_number], "\n")] = '\0';
    check->pattern_number++;
  }
}

// With this function we free dynamic memory
void free_dynamic_memory(const Check *check, char **array) {
  for (int i = 0; i < check->pattern_number; i++) {
    free(array[i]);
  }
  free(array);
}

// With this function we works with files after loops arguments
void process_after_loop(Check *check, const Flag *flag, char *file_array[],
                        char *pattern_array[]) {
  if (check->file_number > 1) {
    for (int i = 0; i < check->file_number; i++) {
      if (check->exit == 0)
        check_path(check, flag, &pattern_array, file_array[i]);
    }
  } else if (check->file_number == 1) {
    if (check->exit == 0)
      check_path(check, flag, &pattern_array, file_array[0]);
  } else {
    if (check->flag == 1 && check->pattern == 1 && check->file == 0) {
      if (check->exit == 0) terminal_input(check, flag, pattern_array);
    }
    if (check->flag == 1 && check->pattern == 0 && check->file == 0) {
      if (check->exit == 0) fprintf(stderr, "grep:" FLAG_ARGUMENT "\n");
    }
  }
}

// With this function we chnage flag status
void change_flag_status(Check *check, Flag *flags, const char *flag) {
  for (int i = 1; flag[i] != '\0'; i++) {
    switch (flag[i]) {
      case 'i':
        activate_flag(check, &flags->i);
        break;
      case 'v':
        activate_flag(check, &flags->v);
        break;
      case 'n':
        activate_flag(check, &flags->n);
        break;
      case 'h':
        activate_flag(check, &flags->h);
        break;
      case 's':
        activate_flag(check, &flags->s);
        break;
      case 'o':
        activate_flag(check, &flags->o);
        break;
      case 'c':
        activate_flag(check, &flags->c);
        break;
      case 'l':
        activate_flag(check, &flags->l);
        break;
      default:
        check->flag_error = 1;
        check->flag = 1;
    }
  }
}

// With this function we idnetify what system it is
void detect_macOS(Check *check) {
#ifdef __APPLE__
  check->macOS = 1;
#else
  check->macOS = 0;
#endif
}

// With this function we check existance of path and type of path(file,
// directory or other)
void check_path(Check *check, const Flag *flag, char ***patterns, char *path) {
  struct stat checkPath;

  if (stat(path, &checkPath) == 0) {
    if (S_ISREG(checkPath.st_mode)) {
      file_input(check, flag, patterns, path);
    } else if (S_ISDIR(checkPath.st_mode)) {
      check->path_error = 1;
    } else {
      check->path_error = 1;
      if (flag->s == 0) error_message(check, path, OTHER);
      if (check->ef_pattern) check->exit = 1;
    }
  } else {
    check->path_error = 1;
    if (flag->s == 0) error_message(check, path, PATH);
    if (check->ef_pattern) check->exit = 1;
  }
}

// With this function we take inputs from file
void file_input(Check *check, const Flag *flag, char ***patterns, char *path) {
  FILE *file = fopen(path, "r");
  if (file != NULL) {
    handle_input_lines(check, flag, file, patterns, path);
    fclose(file);
  } else {
    check->path_error = 1;
    if (flag->s == 0) error_message(check, path, DENIED);
    if (check->ef_pattern) check->exit = 1;
  }
}

// With this function we take inputs from stdin
void terminal_input(Check *check, const Flag *flag, char *arguments[]) {
  char *path = "(standard input)";
  FILE *file = stdin;
  handle_input_lines(check, flag, file, &arguments, path);
}

// With this function we handle input and give outputs based on flgas
void handle_input_lines(Check *check, const Flag *flag, FILE *file,
                        char ***array, char *path) {
  static const char *file_pattern;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  check->counter = 1;
  detect_macOS(check);
  counters_to_zero(check);
  while ((read = getline(&line, &len, file)) != -1) {
    check_and_append_newline(line, &read);
    if (check->ef_pattern == 1) {
      check->file_pattern = 1;
      file_pattern = path;
      adding_pattern_to_array(check, array, line);
    } else {
      if (check->file_pattern == 1) check_same_path(check, path, file_pattern);
      if (flag->l) {
        if (display_file_match_count(check, flag, *array, path, file_pattern,
                                     line))
          break;
      } else if (flag->c) {
        counts_match(check, flag, *array, path, file_pattern, line);
      } else if (flag->v == 1) {
        if (!(check->macOS == 0 && flag->o))
          if (check->same == 0)
            output_dontmatch(check, flag, *array, path, file_pattern, line);
      } else if (flag->o && flag->v == 0) {
        find_pattern_in_line(check, flag, *array, path, file_pattern, line);
      }
      if (vlco_flags_zero(flag))
        output_match(check, flag, *array, path, file_pattern, line);
    }
    check->counter++;
  }
  shows_count(check, flag, path);
  free(line);
}

// With this function we display file when first match or dont match finds if l
// flag is active
int display_file_match_count(Check *check, const Flag *flag, char *array[],
                             const char *path, const char *file_pattern,
                             char *line) {
  int return_number = 0;
  counts_match(check, flag, array, path, file_pattern, line);
  if (check->true_count == 1 || check->false_count == 1) {
    if (check->macOS) change_with_c(check, flag, path);
    if (!(check->same && flag->v)) printf("%s\n", path);
    check->breaker = 1;
    return_number = 1;
  }
  return return_number;
}

// With this function we display count of matching when c flag is active
void shows_count(Check *check, const Flag *flag, const char *path) {
  if (flag->l && flag->c && check->breaker == 0 && check->ef_pattern == 0) {
    if (check->true_count == 0 || check->false_count == 0) {
      change_with_c(check, flag, path);
    }
  }
  if (flag->c && flag->l == 0 && check->ef_pattern == 0)
    change_with_c(check, flag, path);
}

// With this function search patterns and return numbers
int find_pattern_in_line(const Check *check, const Flag *flag, char **array,
                         const char *path, const char *file_pattern,
                         char *line) {
  int return_number = 0;
  int matchResult;
  int start = 0;
  int regFlag = REG_EXT;
  regex_t searchPatterns[check->pattern_number];
  regmatch_t match;
  if (flag->i) regFlag = REG_CASE;
  for (int i = 0; i < check->pattern_number; i++) {
    if (regcomp(&searchPatterns[i], array[i], regFlag)) {
      return_number = -1;
      return return_number;
    }
    if (regexec(&searchPatterns[i], line + start, 0, NULL, 0) == 0) {
      if (flag->o)
        matchResult = regexec(&searchPatterns[i], line + start, 1, &match, 0);
      output_exact_match(check, flag, &match, path, file_pattern, &start,
                         matchResult, line);
      return_number = 1;
      regfree(&searchPatterns[i]);
    }
    regfree(&searchPatterns[i]);
  }
  return return_number;
}

// With this function we display exact matches when o flag is active
void output_exact_match(const Check *check, const Flag *flag,
                        const regmatch_t *match, const char *path,
                        const char *file_pattern, int *start, int exactMatch,
                        const char *line) {
  if (flag->o && vlc_flags_zero(flag) && !exactMatch) {
    add_filepath_prefix(check, flag, path);
    change_with_n(check, flag);
    int match_start = (*start) + match->rm_so;
    int match_end = (*start) + match->rm_eo;
    if (file_pattern != NULL && check->file_pattern &&
        strcmp(path, file_pattern) == 0) {
      printf("%s", line);
    } else {
      printf("%.*s\n", (int)(match->rm_eo - match->rm_so), line + match_start);
    }
    *start = match_end;
  }
}

// With this function we display matches
void output_match(const Check *check, const Flag *flag, char *array[],
                  const char *path, const char *file_pattern, char *line) {
  int match =
      find_pattern_in_line(check, flag, array, path, file_pattern, line);
  if (match) {
    add_filepath_prefix(check, flag, path);
    change_with_n(check, flag);
    printf("%s", line);
  }
}

// With this fuction we display dont matches when v flag is active
void output_dontmatch(const Check *check, const Flag *flag, char *array[],
                      const char *path, const char *file_pattern, char *line) {
  if (find_pattern_in_line(check, flag, array, path, file_pattern, line) != 1) {
    add_filepath_prefix(check, flag, path);
    change_with_n(check, flag);
    printf("%s", line);
  }
}

// With this function dont display if f flag is active
void add_filepath_prefix(const Check *check, const Flag *flag,
                         const char *path) {
  if (check->file_number > 1 && flag->h == 0) {
    printf("%s:", path);
  }
}

// With this function we counting lines when n flag active
void change_with_n(const Check *check, const Flag *flag) {
  if (flag->n == 1) {
    printf("%d:", check->counter);
  }
}

// With this function we display counts if c flag active
void change_with_c(Check *check, const Flag *flag, const char *path) {
  if (flag->c) {
    add_filepath_prefix(check, flag, path);
    if (flag->v) {
      printf("%d\n", check->false_count);
      check->false_count = 0;
    } else {
      printf("%d\n", check->true_count);
      check->true_count = 0;
    }
  }
}

// With this function we check arguments for e and f flags
int check_pattern_for_ef(int argc, const char *argv[]) {
  int return_number = 0;
  for (int i = 0; i < argc; ++i) {
    if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "-f") == 0) {
      return_number = 1;
    }
  }
  return return_number;
}

// With this function check file pattern and path equal
void check_same_path(Check *check, const char *path, const char *path_check) {
  if (check->ef_pattern == 0 && strcmp(path, path_check) == 0) {
    check->same = 1;
  }
}

// With this function we check newline at the end of file if not append it
void check_and_append_newline(char *line, ssize_t *read) {
  if (line[*read - 1] != '\n') {
    line[*read] = '\n';
    line[*read + 1] = '\0';
    (*read)++;
  }
}

// With this function if there is other flags we change status
void activate_flag(Check *check, int *flag) {
  if (*flag == 0) {
    *flag = 1;
    check->flag = 1;
  }
}

// With this function activate some flags in e and f flags
void activate_flags(Check *check, int *flag) {
  *flag = 1;
  check->flag = 1;
  check->pattern = 1;
  check->ef_pattern = 1;
}

// With this function we zero some flags in every file
void counters_to_zero(Check *check) {
  check->false_count = 0;
  check->true_count = 0;
  check->breaker = 0;
  check->same = 0;
}

// With this function we counts match and dont matchs for c flags
void counts_match(Check *check, const Flag *flag, char *array[],
                  const char *path, const char *file_pattern, char *line) {
  check->true_count +=
      find_pattern_in_line(check, flag, array, path, file_pattern, line);
  if (flag->v &&
      find_pattern_in_line(check, flag, array, path, file_pattern, line) != 1)
    check->false_count++;
}

// With this function we check v, l, c and o flags status is zero we give 1
int vlco_flags_zero(const Flag *flag) {
  int vlco_number = 0;
  if (vlc_flags_zero(flag) == 1 && flag->o == 0) {
    vlco_number = 1;
  }
  return vlco_number;
}

// With this function we check v, l and c flags is zero we give 1
int vlc_flags_zero(const Flag *flag) {
  int vlc_number = 0;
  if (flag->v == 0 && flag->l == 0 && flag->c == 0) {
    vlc_number = 1;
  }
  return vlc_number;
}

// With this function we give error messages when errors comes
void error_message(const Check *status, const char *path, const char *type) {
  if (status->no_argument) {
    fprintf(stderr, "grep: %s\n", type);
  }
  if (status->path_error) {
    fprintf(stderr, "grep: %s: %s\n", path, type);
  }
  if (status->flag_error) {
    fprintf(stderr, "grep: %s: %s\n", type, path);
  }
}
