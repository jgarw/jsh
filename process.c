#include "process.h"
#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>

// getCommandType function that will take a command and return the type of command
CommandType getCommandType(char *command) {
    if (strcmp(command, "cd") == 0) return CMD_CD;
    if (strcmp(command, "alias") == 0) return CMD_ALIAS;
    if (strcmp(command, "source") == 0) return CMD_SOURCE;
    if (strcmp(command, "exit") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

// changeDir function that will take a path and change the current working directory
void changeDir(char *path) {
    chdir(path);
}

// parseInput function that will take a string and split it into a command and args
void parseInput(char *input, char **command, char **args) {
    *command = strtok(input, " ");
    *args = strtok(NULL, "");
}

// processCommand function that will take a command and args and execute the command
int processCommand(char *command, char *args){

    char *aliasValue = getAlias(command);
        if (aliasValue) {
            char expandedCommand[MAX_CHAR_LENGTH];
            snprintf(expandedCommand, sizeof(expandedCommand), "%s %s", aliasValue, args ? args : "");
            parseInput(expandedCommand, &command, &args);
        }
    
        // get the command type
        CommandType cmdType = getCommandType(command);
        switch (cmdType) {
            case CMD_EXIT:
                exit(0);
                break;
            case CMD_CD:
                changeDir(args ? args : getenv("HOME"));
                break;
            case CMD_ALIAS:
                if (args) {
                    char *name = strtok(args, "=");
                    char *value = strtok(NULL, "");
                    if (name && value){
                    addAlias(name, value);
                    }else{
                        fprintf(stderr, "Invalid alias command.\n");
                    }
                }
                break;
            case CMD_SOURCE:
                sourceFile(args);
                break;
            case CMD_UNKNOWN:
                externalCommand(command, args);
                break;
            default:
                fprintf(stderr, "Invalid command.\n");
                break;
        }

    return 0;
}

// processInput function that will take a prompt string and process user input
int processInput(char *prompt_str) {

    if (!prompt_str) {
        fprintf(stderr, "Failed to build prompt.\n");
        return 1;
    }

    char *user_input = readline(prompt_str);
    if (!user_input) {
        fprintf(stderr, "Exiting shell...\n");
        return 1;
    }

    if (strlen(user_input) == 0) {
        fprintf(stderr, "Nothing entered.\n");
        return 0;
    }

    user_input[strcspn(user_input, "\n")] = '\0';
    add_history(user_input);

    char *command = NULL;
    char *args = NULL;
    parseInput(user_input, &command, &args);

    // 
    processCommand(command, args);
    return 0;
}

// externalCommand function that will take a command and args and execute the command
int externalCommand(char *command, char *args) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        char *argv[MAX_CHAR_LENGTH];
        argv[0] = command;
        int i = 1;

        if (args) {
            char *token = strtok(args, " ");
            while (token) {
                argv[i++] = token;
                token = strtok(NULL, " ");
            }
        }
        argv[i] = NULL;

        execvp(command, argv);
        perror("Command execution failed");
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, NULL, 0);
    }
    return 0;
}

// create function that will build a prompt string
char *buildPrompt()
{

    int max_prompt_len = 2048;  // Adjust based on expected prompt size
    char *prompt_str = malloc(max_prompt_len);

    // set the cwd variable to the maximum size
    char cwd[MAX_CHAR_LENGTH];
    char *home_dir = getenv("HOME");
    char *user = getenv("USER");
    char branch[MAX_CHAR_LENGTH] = "";

    // Check if the current directory is part of a Git repository
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
    if (fp != NULL)
    {
        if (fgets(branch, sizeof(branch), fp) != NULL)
        {
            // Remove trailing newline from branch name
            branch[strcspn(branch, "\n")] = '\0';
        }
        pclose(fp);
    }

    // get cwd
    getcwd(cwd, sizeof(cwd));

    // create variable to hold relative path string
    char relative_path[MAX_CHAR_LENGTH];
    // if cwd and home_dir are the same up until the length of home_dir, then cwd is under users home dir
    if (strncmp(cwd, home_dir, strlen(home_dir)) == 0)
    {
        // build the relative path by reading cwd starting from where home_dir ends
        snprintf(relative_path, sizeof(relative_path), "~%s", cwd + strlen(home_dir));
    }
    else
    {
        // if not inside the home directory, show the absolute path
        snprintf(relative_path, sizeof(relative_path), "%s", cwd);
    }

    // if there is a git branch, build prompt with branch name
    int len;
    if (strlen(branch) > 0) {
        len = snprintf(prompt_str, max_prompt_len, "\033[34m[%s][%s](\033[33m%s\033[34m)$ \033[0m", user, relative_path, branch);
    } else {
        len = snprintf(prompt_str, max_prompt_len, "\033[34m[%s][%s]$ \033[0m", user, relative_path);
    }

    // Ensure the string was not truncated
    if (len >= max_prompt_len) {
        fprintf(stderr, "Warning: Prompt string was truncated.\n");
    }

    

    return prompt_str;
}

// sourceFile function that will take a filename and execute the commands in the file
int sourceFile(char *filename)
{
    FILE *file = fopen(filename, "r");

    if (!filename) {
        fprintf(stderr, "Error: No file specified for source command.\n");
        return 1;
    }

    if (!file)
    {
        fprintf(stderr, "Failed to open file: %s", filename);
    }

    char line[MAX_CHAR_LENGTH];

    while(fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '#' || strlen(line) == 0) continue;
        char *command = NULL;
        char *args = NULL;
        parseInput(line, &command, &args);
        processCommand(command, args);
        
    }
    fclose(file);
    return 0;
}
    
