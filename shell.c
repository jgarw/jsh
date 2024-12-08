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
#define MAX_HISTORY 100

// define an enum that will be used in switch statement based on commands entered
typedef enum {
    CMD_CD,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

int processInput();
void prompt();
void changeDir(char *path);
void parseInput(char *input, char **command, char **args);
void externalCommand(char *command, char *args);
CommandType getCommandType(char *command);

// function to parse the input into commands and arguments
void parseInput(char *input, char **command, char **args){
    // parse the command from the input string
    *command = strtok(input, " ");
    // parse the arguments from the input string
    *args = strtok(NULL, "");
}

// create a function that returns an enum based on command entered to use in switch
CommandType getCommandType(char *command){
    if (strcmp(command, "cd") == 0){
        return CMD_CD;
    }
    else if (strcmp(command, "exit") == 0){
        return CMD_EXIT;
    }
    else{
        return CMD_UNKNOWN;
    }
}

// function to allow user input that can be processed
int processInput(){

    // set the cwd variable to the maximum size
    char cwd[1024];
    char prompt_str[MAX_CHAR_LENGTH];
    // get cwd
    getcwd(cwd, sizeof(cwd));

    snprintf(prompt_str, sizeof(prompt_str), "\033[32m%s>\033[0m ", cwd);


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

        switch (cmdType){
            case (CMD_EXIT):
                exit(0);
                return 0;
                break;
            
            case (CMD_CD):
                if (args == NULL || strlen(args) == 0){
                    char *home = getenv("HOME");
                    // change directory
                    changeDir(home);
                }
                else{
                    changeDir(args);
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
    // else{
    //     // Handle input errors
    //     perror("Error reading user input");
    //     return 1;
    // }
// }

// function to handle any external commands entered
void externalCommand(char *command, char *args)
{
    pid_t pid = fork();
    if (pid == -1){
        perror("fork");
    }
    else if (pid == 0){
        // create an array to store the arguments
        char *argv[MAX_CHAR_LENGTH];

        // the first element of the array is the command
        argv[0] = command;

        // if there were arguments entered
        if (args != NULL){
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
        else{
            argv[1] = NULL;
        }

        // execute the command and arguments passed
        execvp(command, argv);

        // If execvp returns, it means the command failed
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    else{

        int status;
        waitpid(pid, &status, 0);
    }
}

// function to show the current directory as a prompt
void prompt(){

    // set the cwd variable to the maximum size
    char cwd[1024];
    // get cwd
    getcwd(cwd, sizeof(cwd));

    // print out cwd to console
    printf("\033[32m%s \033[0m> ", cwd);

    processInput();
}

// function to change the current directory
void changeDir(char *path){
    chdir(path);
}

// main function
int main(){

    do{
        processInput();
    } while (1);

    return 0;
}