#include "init.h"
#include "process.h"
#include <stdio.h>

int main() {
    loadConfig();

    while (1) {
        char *prompt_str = buildPrompt();
        if (!prompt_str) {
            fprintf(stderr, "Failed to build prompt.\n");
            break;
        }

        if (processInput(prompt_str) != 0) {
            break;
        }
    }

    return 0;
}
