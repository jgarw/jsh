# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <dirent.h>
# include <stdbool.h>
# include <stdlib.h>
# include <sys/stat.h>

# define MAX_CHAR_LENGTH 1024

// define an enum that will be used in switch statement based on commands entered
typedef enum {
    CMD_LS,
    CMD_CD,
    CMD_MKDIR,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

int processInput();
void prompt();
void listDir(char *path);
void changeDir(char *path);
void makedir(char *dirname);
void parseInput(char *input, char **command, char **args);
CommandType getCommandType(char *command);


// function to parse the input into commands and arguments
void parseInput(char *input, char **command, char **args){
    // parse the command from the input string
    *command = strtok(input, " "); 
    //parse the arguments from the input string
    *args = strtok(NULL, "");
}

// create a function that returns an enum based on command entered to use in switch
CommandType getCommandType(char *command){
    // return CMS_LS if command is "ls"
    if(strcmp(command, "ls") == 0){ return CMD_LS; }
    else if(strcmp(command, "cd") == 0){ return CMD_CD; }
    else if(strcmp(command, "mkdir") == 0){ return CMD_MKDIR; }
    else if(strcmp(command, "exit") == 0){return CMD_EXIT; }
    else { return CMD_UNKNOWN; }
}

// function to allow user input that can be processed
int processInput(){

    char user_input[MAX_CHAR_LENGTH];

   // Use fgets to read input
    if (fgets(user_input, sizeof(user_input), stdin) != NULL) {

        // Remove newline character if present at the end of the input
        user_input[strcspn(user_input, "\n")] = '\0';

        // create pointer to command and args
        char *command = NULL;
        char *args = NULL;
        parseInput(user_input, &command, &args);

        CommandType cmdType = getCommandType(command);

        switch (cmdType){

            case(CMD_EXIT):
                exit(0);
                return 0;
                break;

            case (CMD_LS):
                // if no arguments are passed with ls command then list cwd
                if((args == NULL) || strlen(args) == 0){
                    // list the contents of the current directory
                    listDir(".");
                }
                // if arguments are passed, list directory in arguments
                else{
                    listDir(args);
                }
                break;
            
            case (CMD_CD):

                if(args == NULL || strlen(args) == 0){

                    char *home = getenv("HOME");
                    //change directory
                    changeDir(home);
                }
                else {
                    changeDir(args);
                }

                break;

            case (CMD_MKDIR):

                if(args == NULL || strlen(args) == 0){
                    printf("Enter desired directory name: ");

                    char mkdirName[MAX_CHAR_LENGTH];

                    if (fgets(mkdirName, sizeof(mkdirName), stdin) != NULL) {

                        //remove newline character from directory name entered
                        mkdirName[strcspn(mkdirName, "\n")] = '\0';
                        //create directory from name entered
                        makedir(mkdirName);
                    }else{
                        fprintf(stderr, "Error reading directory name.\n");
                    }
                }
                else {
                        // if arguments are passed, create directory with argument as directory name
                        makedir(args);
                    }
                break;

            case (CMD_UNKNOWN):
                fprintf(stderr, "unknown command entered.\n");
                prompt();
        }

        return 0;

    } else {
        // Handle input errors
        perror("Error reading user input");
        return 1;
    }
}

// function to list the contents of the current directory
void listDir(char *path) {

    // create a pointer to a directory entry
    struct dirent *entry;

    // create a pointer to a directory stream
    DIR *dp;

    // open the directory stream
    dp = opendir(path);

    // check if the directory stream is null
    if (dp == NULL) {
        perror("opendir");
        return;
    }

    // read the directory entries
    while ((entry = readdir(dp))) {
        printf("%s  ", entry->d_name);
    }

    printf("\n");

    // Close the directory
    closedir(dp);
}

// function to show the current directory as a prompt
void prompt(){

    // set the cwd variable to the maximum size
    char cwd[1024];
    //get cwd
    getcwd(cwd, sizeof(cwd));

    //print out cwd to console
    printf("%s> ", cwd);

    processInput();
}

// function to change the current directory
void changeDir(char *path){
    chdir(path);
}

// function to create a new directory
void makedir(char *dirname){

    // create directory with 755 permissions
    mkdir(dirname, 0755);
}

// main function
int main(){

    do {
        prompt();
    } while (1);

    return 0;
}