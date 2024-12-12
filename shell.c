#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_CHAR_LENGTH 1024
#define MAX_ALIASES 50
#define MAX_ALIAS_NAME 50
#define MAX_ALIAS_VALUE 1024

// define an enum that will be used in switch statement based on commands entered
typedef enum
{
    CMD_CD,
    CMD_LL,
    CMD_ALIAS,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef struct
{
    char name[MAX_ALIAS_NAME];
    char value[MAX_ALIAS_VALUE];
} Alias;

// Array to store aliases
Alias aliases[MAX_ALIASES];
int alias_count = 0;

int processInput();
void prompt();
void changeDir(char *path);
void parseInput(char *input, char **command, char **args);
void externalCommand(char *command, char *args);
CommandType getCommandType(char *command);
void addAlias(char *name, char *value);
char *getAlias(char *name);

// create a function that returns an enum based on command entered to use in switch
CommandType getCommandType(char *command)
{
    if (strcmp(command, "cd") == 0)
    {
        return CMD_CD;
    }
    else if (strcmp(command, "ll") == 0)
    {
        return CMD_LL;
    }
    else if (strcmp(command, "alias") == 0)
    {
        return CMD_ALIAS;
    }
    else if (strcmp(command, "exit") == 0)
    {
        return CMD_EXIT;
    }
    else
    {
        return CMD_UNKNOWN;
    }
}

// create a funciton that allows user to create aliases
void addAlias(char *name, char *value)
{

    // check if there is already the max number of aliases created
    if (alias_count >= MAX_ALIASES)
    {
        printf("Maximum number of aliases already exists.");
        return;
    }
    // if not check if the alias name already exists
    else
    {

        // iterate through the list of alias names
        for (int i = 0; i < alias_count; i++)
        {
            // if the name entered matches an existing alias name, replace the alias value with the new value
            if (strcmp(aliases[i].name, name) == 0)
            {
                strcpy(aliases[i].value, value);
                return;
            }
        }

        // if the alias name doesnt exist yet, copy entered name and value into alias list
        strcpy(aliases[alias_count].name, name);

        // Remove surrounding quotes if present
        if (value[0] == '"' && value[strlen(value) - 1] == '"')
        {
            value[strlen(value) - 1] = '\0';               // Remove trailing quote
            strcpy(aliases[alias_count].value, value + 1); // Skip leading quote
        }
        else
        {
            strcpy(aliases[alias_count].value, value);
        }

        // print a message stating that alias was created
        printf("Alias %s='%s' created.\n", aliases[alias_count].name, aliases[alias_count].value);

        alias_count++;
    }
}

// function to retrieve an alias from the list
char *getAlias(char *name)
{
    // iterate through the list of aliases
    for (int i = 0; i < alias_count; i++)
    {
        // if an alias with a matching name is found, return the value
        if (strcmp(aliases[i].name, name) == 0)
        {
            return aliases[i].value;
        }
    }

    return NULL;
}

// function to parse the input into commands and arguments
void parseInput(char *input, char **command, char **args)
{
    // parse the command from the input string
    *command = strtok(input, " ");
    // parse the arguments from the input string
    *args = strtok(NULL, "");
}

// function to allow user input that can be processed
int processInput()
{

    // set the cwd variable to the maximum size
    char cwd[1024];
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

    // create variable used to build prompt string
    char prompt_str[MAX_CHAR_LENGTH];
    // get cwd
    getcwd(cwd, sizeof(cwd));

    // if there is a git branch, build prompt with branch name
    if (strlen(branch) > 0)
    {
        snprintf(prompt_str, sizeof(prompt_str), "\033[34m[%s](\033[33m%s\033[34m)$ \033[0m", cwd, branch);
    }
    else
    {
        snprintf(prompt_str, sizeof(prompt_str), "\033[34m[%s]$ \033[0m", cwd);
    }

    // use readline to handle user input
    char *user_input = readline(prompt_str);

    // Remove newline character if present at the end of the input
    user_input[strcspn(user_input, "\n")] = '\0';
    add_history(user_input);
    // create pointer to command and args
    char *command = NULL;
    char *args = NULL;
    parseInput(user_input, &command, &args);
    CommandType cmdType = getCommandType(command);

    // Check if the command is an alias
    char *aliasValue = getAlias(command);
    if (aliasValue != NULL)
    {
        char expandedCommand[MAX_CHAR_LENGTH];
        if (args != NULL)
        {
            snprintf(expandedCommand, sizeof(expandedCommand), "%s %s", aliasValue, args);
        }
        else
        {
            snprintf(expandedCommand, sizeof(expandedCommand), "%s", aliasValue);
        }

        // Now parse the expanded command
        parseInput(expandedCommand, &command, &args);
        // update cmdType with expanded command from alias
        cmdType = getCommandType(command);
    }

    switch (cmdType)
    {
    case (CMD_EXIT):
        exit(0);
        return 0;
        break;

    case (CMD_CD):
        if (args == NULL || strlen(args) == 0)
        {
            char *home = getenv("HOME");
            // change directory
            changeDir(home);
        }
        else
        {
            changeDir(args);
        }
        break;

    case (CMD_LL):
        command = "ls";
        args = "-al";
        externalCommand(command, args);
        break;

    case (CMD_ALIAS):
        if (args != NULL)
        {

            char *name = strtok(args, "=");
            char *value = strtok(NULL, "");

            addAlias(name, value);
        }

        break;

    case (CMD_UNKNOWN):
        // fprintf(stderr, "unknown command entered.\n");
        // prompt();
        externalCommand(command, args);
        break;
    }
    return 0;
}

// function to handle any external commands entered
void externalCommand(char *command, char *args)
{

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
    }
    else if (pid == 0)
    {
        // create an array to store the arguments
        char *argv[MAX_CHAR_LENGTH];

        // the first element of the array is the command
        argv[0] = command;

        // if there were arguments entered
        if (args != NULL)
        {
            // create variable to hold arguments by seperating arguments entered by spaces between them
            char *token = strtok(args, " ");

            int i = 1;
            // iterate through the arguments passed
            while (token != NULL)
            {
                // store each argument(token) in the argument array(argv)
                argv[i++] = token;
                // return the next argument/token from the string entered
                token = strtok(NULL, " ");
            }

            argv[i] = NULL;
        }
        else
        {
            argv[1] = NULL;
        }

        // execute the command and arguments passed
        execvp(command, argv);

        // If execvp returns, it means the command failed
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    else
    {

        int status;
        waitpid(pid, &status, 0);
    }
}

// function to change the current directory
void changeDir(char *path)
{
    chdir(path);
}

// main function
int main()
{

    do
    {
        processInput();
    } while (1);

    return 0;
}