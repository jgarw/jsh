#ifndef INIT_H
#define INIT_H

#define MAX_CHAR_LENGTH 1024
#define MAX_ALIASES 100
#define MAX_ALIAS_NAME 50
#define MAX_ALIAS_VALUE 1024

typedef struct {
    char name[MAX_ALIAS_NAME];
    char value[MAX_ALIAS_VALUE];
} Alias;

extern Alias aliases[MAX_ALIASES];
extern int alias_count;

void loadConfig();
char *buildPrompt();
void addAlias(char *name, char *value);
char *getAlias(char *name);

#endif
