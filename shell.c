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
#define MAX_ALIASES 100
#define MAX_ALIAS_NAME 50
#define MAX_ALIAS_VALUE 1024

// define an enum that will be used in switch statement based on commands entered
typedef enum
{
    CMD_CD,
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
char *buildPrompt();
void loadConfig();
void changeDir(char *path);
void parseInput(char *input, char **command, char **args);
int externalCommand(char *command, char *args);
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

// create a function to load the contents of .jshrc on shell start
void loadConfig(){

    // get the home directory location
    char *home = getenv("HOME");

    char location[MAX_CHAR_LENGTH];

    snprintf(location, sizeof(location), "%s/.jshrc", home);

    // read the config file at the location
    FILE *file = fopen(location, "r");
    // handle case of no .jshrc file
    if (file == NULL)
    {
        fprintf(stderr, "\033[31mNo .jshrc file found in %s.\n", location);
        file = fopen(location, "w");

        printf("\033[33mCreating .jshrc file in your home directory...\n\033[0m");
        fprintf(file, "# This is your jsh config file. Enter aliases here.\n# Example:\n");
        // enter an example alias 
        fprintf(file, "alias ll='ls -al'\n");
        fclose(file);
        
        return;
    }
    
    // store lines read
    char line[MAX_CHAR_LENGTH];
    while(fgets(line, sizeof(line), file) != NULL){

        line[strcspn(line, "\n")] = '\0';

        // skip lines that start with # (comments)
        if(line[0] == '#'){
            continue;
        }

        // skip empty lines
        if(strlen(line) == 0){
            continue;
        }

        // Parse alias lines (e.g., "alias ll='ls -al'")
        if (strncmp(line, "alias ", 6) == 0)
        {
            char *alias_part = line + 6;  // Skip the "alias " part
            char *name = strtok(alias_part, "=");
            char *value = strtok(NULL, "");

            if (name && value)
            {
                // Remove surrounding quotes if present
                if ((value[0] == '\'' && value[strlen(value) - 1] == '\'') ||
                    (value[0] == '"' && value[strlen(value) - 1] == '"'))
                {
                    value[strlen(value) - 1] = '\0'; // Remove trailing quote
                    strcpy(aliases[alias_count].value, value + 1);
                }
                else
                {
                    strcpy(aliases[alias_count].value, value);
                }

                strcpy(aliases[alias_count].name, name);
                alias_count++;
            }
        }
    }

    fclose(file);


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
        if ((value[0] == '\'' && value[strlen(value) - 1] == '\'') || (value[0] == '"' && value[strlen(value) - 1] == '"'))
        {
            value[strlen(value) - 1] = '\0';               // Remove trailing quote
            strcpy(aliases[alias_count].value, value + 1); // Skip leading quote
        }
        else
        {
            strcpy(aliases[alias_count].value, value);
        }

        // print a message stating that alias was created
        //printf("Alias %s='%s' created.\n", aliases[alias_count].name, aliases[alias_count].value);

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

// create function that will build a prompt string
char *buildPrompt()
{

    static char prompt_str[MAX_CHAR_LENGTH];

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
        if (strncmp(cwd, home_dir, strlen(home_dir)) == 0){
            // build the relative path by reading cwd starting from where home_dir ends
            snprintf(relative_path, sizeof(relative_path), "~%s", cwd + strlen(home_dir));
        }

        // if there is a git branch, build prompt with branch name
        if (strlen(branch) > 0)
        {
            snprintf(prompt_str, sizeof(prompt_str), "\033[34m[%s][%s](\033[33m%s\033[34m)$ \033[0m", user, relative_path, branch);
        }
        else
        {
            snprintf(prompt_str, sizeof(prompt_str), "\033[34m[%s][%s]$ \033[0m", user, relative_path);
        }

    return prompt_str;
}

// function to allow user input that can be processed
int processInput()
{

    // create pointer to built prompt string
    char *prompt_str = buildPrompt();

    // handle errors for building prompt
    if (prompt_str == NULL){
        fprintf(stderr, "Unable to build prompt.");
        return 1;   
    }

    // use readline to handle user input
    char *user_input = readline(prompt_str);

    // handle case of no user input
    if(user_input == NULL){
        fprintf(stderr, "Exiting shell...\n");
        return 1;
    }

    if(strlen(user_input) == 0){
        fprintf(stderr, "Nothing entered. nothing to do...\n");
        return 0;
    }

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

    // if command has an alias
    if (aliasValue != NULL)
    {
        char expandedCommand[MAX_CHAR_LENGTH];

        // if arguments entered with alias, break up alias into command and args
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

    case (CMD_ALIAS):
        if (args != NULL)
        {

            char *name = strtok(args, "=");
            char *value = strtok(NULL, "");

            addAlias(name, value);
        }

        break;

    case (CMD_UNKNOWN):
        externalCommand(command, args);
        break;

    default:
        fprintf(stderr, "Unrecognized command type\n");
        return 1;
        break;
    }
    return 0;
}

// function to handle any external commands entered
int externalCommand(char *command, char *args)
{

    // create full command to check for pipe
    char fullCommand[MAX_CHAR_LENGTH];

    if(args != NULL){
        snprintf(fullCommand, sizeof(fullCommand), "%s %s", command, args);
    }
    else{
        snprintf(fullCommand, sizeof(fullCommand), "%s", command);
    }

    // handle if command entered has a pipe 
    char *pipePosition = strchr(fullCommand, '|');
    if(pipePosition != NULL){

        // terminate first command
        *pipePosition = '\0';

        // split the entered command into 2 seperate commands
        char *firstCommand = fullCommand;
        char *secondCommand = pipePosition + 1;

        int fd[2];

        if(pipe(fd) == -1){
            perror("Pipe Error");
            return 1;
        }

        pid_t pid1 = fork();

        // if the fork failed
        if(pid1 == -1){
            perror("Fork error");
            return 1;
            exit(EXIT_FAILURE);
        }

        // inside child process
        if(pid1 == 0){
            // close the read end of the pipe
            close(fd[0]);
            // redirect to pipe read end
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);

            char *argv1[] = {"/bin/sh", "-c", firstCommand, NULL};
            execvp(argv1[0], argv1);

            // if execvp returns then an error occured
            perror("Error executing first command in pipe operation.");
            return 1;
            exit(EXIT_FAILURE);
        }

        pid_t pid2 = fork();

        // if the fork failed
        if(pid2 == -1){
            perror("Fork error");
            return 1;
            exit(EXIT_FAILURE);
        }

        if(pid2 == 0){
            // close the read end of the pipe
            close(fd[1]);
            // redirect to pipe write end
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);

            char *argv2[] = {"/bin/sh", "-c", secondCommand, NULL};
            execvp(argv2[0], argv2);

            // if execvp returns then an error occured
            perror("Error executing second command in pipe operation.");
            return 1;
            exit(EXIT_FAILURE);
        }

        close(fd[0]);
        close(fd[1]);
        wait(NULL);
        wait(NULL);
        return 0;
    }

    else{

        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork failed.\n");
            return 1;
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
            fprintf(stderr, "Unknown Command: %s\n", command);
            return 1;
            exit(EXIT_FAILURE);
        }

        else
        {
            // wait for child preocess to finish
            int status;
            waitpid(pid, &status, 0);
            return 0;
        }
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
    loadConfig();
    while(1)
    {
        if(processInput() != 0){
            break;
        }
    } 
    return 0;
}