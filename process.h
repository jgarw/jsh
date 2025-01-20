#ifndef PROCESS_H
#define PROCESS_H

#include "init.h"

// CommandType enum that will define the different types of commands
typedef enum {
    CMD_CD,
    CMD_ALIAS,
    CMD_SOURCE,
    CMD_PIPE,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

void changeDir(char *path);
void parseInput(char *input, char **command, char **args);
int processCommand(char *command, char *args);
int executePipe(char *input);
int processInput();
int externalCommand(char *command, char *args);
// char *buildPrompt();
CommandType getCommandType(char *command);
int sourceFile(char *filename);

#endif
