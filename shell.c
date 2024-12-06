# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <dirent.h>
# include <stdbool.h>
# include <stdlib.h>
# include <sys/stat.h>

# define MAX_CHAR_LENGTH 1024

int acceptInput();
void showCWD();
void listDir(char *path);
void changeDir(char *path);
void makedir(char *dirname);
void parseInput(char *input, char **command, char **args);


// function to parse the input into commands and arguments
void parseInput(char *input, char **command, char **args){

    // parse the command from the input string
    *command = strtok(input, " "); 

    //parse the arguments from the input string
    *args = strtok(NULL, "");


}

// function to allow user input that can be processed
int acceptInput(){

    char user_input[MAX_CHAR_LENGTH];

   // Use fgets to read input
    if (fgets(user_input, sizeof(user_input), stdin) != NULL) {

        // Remove newline character if present at the end of the input
        user_input[strcspn(user_input, "\n")] = '\0';

        char *command = NULL;
        char *args = NULL;
        parseInput(user_input, &command, &args);

        // Check if the user input is "exit" or "quit"
        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
            // Exit the program
            exit(0);
            return 0;
        }
        // Check if the user input is "ls"
        else if (strcmp(command, "ls") == 0) {
            // if no arguments are passed with ls command then list cwd
            if((args == NULL) || strlen(args) == 0){
            // list the contents of the current directory
            listDir(".");
            }
            // if arguments are passed, list directory in arguments
            else{
                listDir(args);
            }
        } 

        // if user enters cd
        else if(strcmp(user_input, "cd") == 0){
            // if no arguments are passed with cd command, cd back to home dir of current user
            if(args == NULL || strlen(args) ==0){
            
            char *home = getenv("HOME");
            //change directory
            changeDir(home);
            }
            else {
                changeDir(args);
            }
        }

        // if user enters mkdir
        else if(strcmp(user_input, "mkdir") == 0){
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
            else{
                    // if arguments are passed, create directory with argument as directory name
                    makedir(args);
                }
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
void showCWD(){

    // set the cwd variable to the maximum size
    char cwd[1024];
    //get cwd
    getcwd(cwd, sizeof(cwd));

    //print out cwd to console
    printf("%s> ", cwd);

    acceptInput();
}

// function to change the current directory
void changeDir(char *path){
    chdir(path);
}

// function to create a new directory
void makedir(char *dirname){
    mkdir(dirname, 0755);
}

// main function
int main(){

    do {
        showCWD();
    } while (1);

    return 0;
}