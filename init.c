#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        if (line[0] == '#' || strlen(line) == 0) continue;

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
