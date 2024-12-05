# include <stdio.h>
# include <unistd.h>
# include <string.h>

# define MAX_CHAR_LENGTH 1024

// function to allow user input that can be processed
int acceptInput(){

    char user_input[MAX_CHAR_LENGTH];

   // Use fgets to read input
    if (fgets(user_input, sizeof(user_input), stdin) != NULL) {

        // Remove newline character if present at the end of the input
        user_input[strcspn(user_input, "\n")] = '\0';
        
        // Print user input back to the user
        printf("You entered: %s\n", user_input);

        return 0;
    } else {
        // Handle input errors
        perror("Error reading user input");
        return 1;
    }
}

// function to show the current directory as a prompt
void showDirectory(){

    // set the cwd variable to the maximum size
    char cwd[1024];
    //get cwd
    getcwd(cwd, sizeof(cwd));

    //print out cwd to console
    printf("%s> ", cwd);

    acceptInput();
}



int main(){

    showDirectory();
    return 0;
}