# Default target
jsh: main.c init.c process.c
	gcc -Wall -Wextra -g main.c init.c process.c -o jsh -lreadline

# Clean target
clean:
	rm -f jsh

.PHONY: clean
