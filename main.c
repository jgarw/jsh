#include "init.h"
#include "process.h"

int main() {
    loadConfig();
    while (1) {
        if (processInput() != 0) break;
    }
    return 0;
}
