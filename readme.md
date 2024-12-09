# 🚀 Custom Unix Shell

This project is a custom Unix shell implemented in C, designed to replicate and enhance core shell functionalities with a user-friendly experience. It supports built-in commands, external command execution, and dynamic Git branch prompts.

## 📋 Features

- **Core Shell Commands:**
  - Supports built-in commands like `cd` and `exit`.
  - Executes external commands with argument handling and process management.

- **Dynamic Git Branch Prompt:**
  - Displays the current Git branch in the prompt if the working directory is inside a Git repository.

- **Command History:**
  - Supports command history using the `readline` library, allowing easy retrieval of past commands.

- **Error Handling:**
  - Provides error messages for failed commands and handles edge cases gracefully.

## 🛠️ Installation

### 1. Dependencies 📦

Ensure the following dependencies are installed:

- **GCC Compiler:**  
  - apt:
    ```shell
    sudo apt install build-essential
    ```
  - Homebrew:
    ```shell
    brew install gcc
    ```
  - dnf:
    ```shell
    sudo dnf install gcc
    ```
- **Readline:**
  - apt:
    ```shell
    sudo apt install libreadline-dev
    ```
  - Homebrew:
    ```shell
    brew install readline
    ```
  - dnf:
    ```shell
    sudo dnf install readline-devel
    ```

### 2. Build the Shell 🔨

Clone the repository and build the shell:
```shell
git clone <repository-url>
cd <project-folder>
gcc shell.c -o jsh -lreadline
```

### 3. Run the Shell 💻
```shell
./jsh
```

## Upcoming Features

- **Syntax Highlighting**
- **Installation Script**
- **Aliasing**
