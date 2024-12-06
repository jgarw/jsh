# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <dirent.h>

# define MAX_CHAR_LENGTH 1024

int acceptInput();
void showCWD();
void listDir(char *path);


// function to allow user input that can be processed
int acceptInput(){

    char user_input[MAX_CHAR_LENGTH];

   // Use fgets to read input
    if (fgets(user_input, sizeof(user_input), stdin) != NULL) {

        // Remove newline character if present at the end of the input
        user_input[strcspn(user_input, "\n")] = '\0';
        
        // Print user input back to the user
        printf("You entered: %s\n", user_input);

        if (strcmp(user_input, "ls") == 0) {
            // Handle some_command
            listDir(".");
        } else {
            // Handle other commands or default case
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

    // Close the directory
    closedir(dp);
}



// function to show the current directory as a prompt
// function to allow user input that can be processed
int acceptInput();

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

int main(){

    showCWD();
    return 0;
}