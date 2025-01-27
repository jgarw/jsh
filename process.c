#include "process.h"
#include "init.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// getCommandType function that will take a command and return the type of command
CommandType getCommandType(char *command) {
    if (strcmp(command, "cd") == 0)
        return CMD_CD;
    if (strcmp(command, "alias") == 0)
        return CMD_ALIAS;
    if (strcmp(command, "source") == 0)
        return CMD_SOURCE;
    if (strcmp(command, "exit") == 0)
        return CMD_EXIT;
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
int processCommand(char *command, char *args) {

    char *aliasValue = getAlias(command);

    char expandedCommand[MAX_CHAR_LENGTH];
    snprintf(expandedCommand, sizeof(expandedCommand), "%s %s", aliasValue, args ? args : "");

    if (aliasValue) {
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
            if (name && value) {
                addAlias(name, value);
            } else {
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

// function to handle any external commands entered
int externalCommand(char *command, char *args) {

    // create full command to check for pipe
    char fullCommand[MAX_CHAR_LENGTH];

    if (args != NULL) {
        snprintf(fullCommand, sizeof(fullCommand), "%s %s", command, args);
    } else {
        snprintf(fullCommand, sizeof(fullCommand), "%s", command);
    }

    // handle if command entered has a pipe
    char *pipePosition = strchr(fullCommand, '|');
    if (pipePosition != NULL) {

        *pipePosition = '\0';

        // split the entered command into 2 seperate commands
        char *firstCommand = fullCommand;
        char *secondCommand = pipePosition + 1;

        int fd[2];

        if (pipe(fd) == -1) {
            perror("Pipe Error");
            return 1;
            exit(EXIT_FAILURE);
        }

        pid_t pid1 = fork();

        // if the fork failed
        if (pid1 == -1) {
            perror("Fork error");
            return 1;
            exit(EXIT_FAILURE);
        }

        if (pid1 == 0) {
            // close the read end of the pipe
            close(fd[0]);
            // redirect to pipe read end
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);

            char *argv1[MAX_CHAR_LENGTH];
            parseInput(firstCommand, &argv1[0], &argv1[1]);
            execvp(argv1[0], argv1);

            // if execvp returns then an error occured
            perror("Error executing first command in pipe operation.");
            return 1;
            exit(EXIT_FAILURE);
        }

        pid_t pid2 = fork();

        // if the fork failed
        if (pid2 == -1) {
            perror("Fork error");
            return 1;
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {
            // close the read end of the pipe
            close(fd[1]);
            // redirect to pipe write end
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);

            char *argv2[MAX_CHAR_LENGTH];
            parseInput(secondCommand, &argv2[0], &argv2[1]);
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

    //handle if command has > redirect
    char *outputPosition = strchr(fullCommand, '>');
    if (outputPosition != NULL){

        // seperate at > character
        *outputPosition = '\0';

        // command is input before >
        char *commandPart = fullCommand;
        //file is input after >
        char *outputFile = outputPosition + 1;

        // remove white space from file name
        outputFile += strspn(outputFile, " ");
        
        if(outputFile != NULL){

            pid_t pid = fork();

            // if fork fails
            if(pid == -1){
                perror("Fork failed during output redirect process.");
                return 1;
            }

            // fork was successful
            if(pid == 0){

                // open the specified file/create if non existent
                int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                //if file failed to open or create
                if(fd == -1){
                    perror("Error while opening file.");
                    exit(EXIT_FAILURE);
                }

                // redirect std output to the file
                dup2(fd, STDOUT_FILENO);
                close(fd);

                // create array for command and arguments
                char *argv[MAX_CHAR_LENGTH];
                // store parsed command and arguments into array
                parseInput(commandPart, &argv[0], &argv[1]);

                // execute the command and arguments with std output redirected to file
                execvp(argv[0], argv);

                
                // If execvp fails
                perror("Command execution failed");
                exit(EXIT_FAILURE);
                
            } else {

                // In the parent process
                int status;
                waitpid(pid, &status, 0);
                return 0;
            }
        
        }

    }

    else {

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed.\n");
            return 1;
        } else if (pid == 0) {
            // create an array to store the arguments
            char *argv[MAX_CHAR_LENGTH];

            // the first element of the array is the command
            argv[0] = command;

            // if there were arguments entered
            if (args != NULL) {
                // create variable to hold arguments by seperating arguments entered by spaces between them
                char *token = strtok(args, " ");

                int i = 1;
                // iterate through the arguments passed
                while (token != NULL) {
                    // store each argument(token) in the argument array(argv)
                    argv[i++] = token;
                    // return the next argument/token from the string entered
                    token = strtok(NULL, " ");
                }

                argv[i] = NULL;
            } else {
                argv[1] = NULL;
            }

            // execute the command and arguments passed
            execvp(command, argv);

            // If execvp returns, it means the command failed
            fprintf(stderr, "Unknown Command: %s\n", command);
            return 1;
            exit(EXIT_FAILURE);
        }

        else {
            // wait for child preocess to finish
            int status;
            waitpid(pid, &status, 0);
            return 0;
        }
    }
}

// sourceFile function that will take a filename and execute the commands in the file
int sourceFile(char *filename) {
    FILE *file = fopen(filename, "r");

    if (!filename) {
        fprintf(stderr, "Error: No file specified for source command.\n");
        return 1;
    }

    if (!file) {
        fprintf(stderr, "Failed to open file: %s", filename);
    }

    char line[MAX_CHAR_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '#' || strlen(line) == 0)
            continue;
        char *command = NULL;
        char *args = NULL;
        parseInput(line, &command, &args);
        processCommand(command, args);
    }
    fclose(file);
    return 0;
}
