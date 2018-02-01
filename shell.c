#include "shell.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"

int main(int argc, char** argv) {
  static char cmd[CMD_LEN];
  FILE* out = stdout;
  // parse command line arguments
  cmd_args args = parse_args(argc, argv);

  // start shell
  while (should_be_open(cmd)) {
    // get input
    print_prompt(args.prompt);
    get_input(cmd);

    // process input
    if (handle_builtin(out, cmd)) continue;

    // test - to be removes
    // print_prompt(args.prompt);
    char temp[100];
    snprintf(temp, 100, "You typed '%s'", cmd);
    write_to_out(out, temp);
  }

  return 0;
}

void print_prompt(const char* prompt) { printf("%s> ", prompt); }

cmd_args parse_args(int argc, char** argv) {
  cmd_args args;
  args.prompt = "308sh";

  for (int i = 1; i < argc; i++) {
    // check for -p flag
    if (strncmp("-p", argv[i], 3) == 0) {
      if (i + 1 < argc) {
        args.prompt = argv[i + 1];
      } else {
        fprintf(stderr, "%s %serror:%s %s", argv[0], KRED, RESET,
                "shell: -p must take a string argument\n");
      }
    }
  }

  return args;
}

bool should_be_open(const char* cmd) {
  const char* exit = "exit";
  return strncmp(exit, cmd, strlen(exit)) != 0;
}

void get_input(char* cmd) {
  fgets(cmd, CMD_LEN - 1, stdin);
  int len = strlen(cmd);
  if (len > 0) cmd[len - 1] = 0;
}

bool handle_builtin(FILE* dest, const char* cmd) {
  if (strncmp("pid", cmd, 4) == 0) {
    builtin_val out_str = pid();
    return process_builtin_out(dest, out_str);
  }

  if (strncmp("ppid", cmd, 5) == 0) {
    builtin_val out_str = ppid();
    return process_builtin_out(dest, out_str);
  }

  if (strncmp("pwd", cmd, 4) == 0) {
    builtin_val out_str = pwd();
    return process_builtin_out(dest, out_str);
  }

  if (strncmp("get ", cmd, 4) == 0) {
    size_t var_length = strlen(cmd) - 3;
    char var[var_length];
    strncpy(var, cmd + 4, var_length);

    builtin_val out_str = get(var);
    return process_builtin_out(dest, out_str);
  }

  if (strncmp("cd ", cmd, 3) == 0) {  // TODO
    size_t var_length = strlen(cmd) - 2;
    char path[var_length];
    // if (path)
    strncpy(path, cmd + 3, var_length);

    builtin_val out_str = cd(path);
    return process_builtin_out(dest, out_str);
  }

  if (strncmp("set ", cmd, 4) == 0) {
    size_t var_length = strlen(cmd) - 3;
    char var[var_length];
    strncpy(var, cmd + 4, var_length);

    const char* env_name = strtok(var, " ");
    const char* env_value = strtok(0, " ");
    builtin_val out_str = set(env_name, env_value);
    return process_builtin_out(dest, out_str);
  }

  return false;
}

bool process_builtin_out(FILE* dest, builtin_val res) {
  write_to_out(dest, res);
  free((void*)res);
  return true;
}

void write_to_out(FILE* dest, const char* out) { fprintf(dest, "%s\n", out); }