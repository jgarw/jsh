#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Alias aliases[MAX_ALIASES];
int alias_count = 0;

void loadConfig() {
    char *home = getenv("HOME");
    char location[MAX_CHAR_LENGTH];
    snprintf(location, sizeof(location), "%s/.jshrc", home);

    FILE *file = fopen(location, "r");
    if (!file) {
        fprintf(stderr, "\033[31mNo .jshrc file found in %s.\n", location);
        file = fopen(location, "w");
        printf("\033[33mCreating .jshrc file...\n\033[0m");
        fprintf(file, "# This is your jsh config file.\n");
        fclose(file);
        return;
    }

    char line[MAX_CHAR_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '#' || strlen(line) == 0)
            continue;

        if (strncmp(line, "alias ", 6) == 0) {
            char *alias_part = line + 6;
            char *name = strtok(alias_part, "=");
            char *value = strtok(NULL, "");
            if (name && value) {
                if ((value[0] == '\'' && value[strlen(value) - 1] == '\'') ||
                    (value[0] == '"' && value[strlen(value) - 1] == '"')) {
                    value[strlen(value) - 1] = '\0';
                    strcpy(aliases[alias_count].value, value + 1);
                } else {
                    strcpy(aliases[alias_count].value, value);
                }
                strcpy(aliases[alias_count].name, name);
                alias_count++;
            }
        }
    }
    fclose(file);
}

// create function that will build a prompt string
char *buildPrompt() {

    int max_prompt_len = 2048; // Adjust based on expected prompt size
    char *prompt_str = malloc(max_prompt_len);

    // set the cwd variable to the maximum size
    char cwd[MAX_CHAR_LENGTH];
    char *home_dir = getenv("HOME");
    char *user = getenv("USER");
    char branch[MAX_CHAR_LENGTH] = "";

    // Check if the current directory is part of a Git repository
    FILE *fp = popen("git rev-parse --abbrev-ref HEAD 2>/dev/null", "r");
    if (fp != NULL) {
        if (fgets(branch, sizeof(branch), fp) != NULL) {
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
    if (strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
        // build the relative path by reading cwd starting from where home_dir ends
        snprintf(relative_path, sizeof(relative_path), "~%s", cwd + strlen(home_dir));
    } else {
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

void addAlias(char *name, char *value) {
    if (alias_count >= MAX_ALIASES) {
        printf("Maximum number of aliases reached.\n");
        return;
    }
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            strcpy(aliases[i].value, value);
            return;
        }
    }
    strcpy(aliases[alias_count].name, name);
    if ((value[0] == '\'' && value[strlen(value) - 1] == '\'') ||
        (value[0] == '"' && value[strlen(value) - 1] == '"')) {
        value[strlen(value) - 1] = '\0';
        strcpy(aliases[alias_count].value, value + 1);
    } else {
        strcpy(aliases[alias_count].value, value);
    }
    alias_count++;
}

char *getAlias(char *name) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            return aliases[i].value;
        }
    }
    return NULL;
}
