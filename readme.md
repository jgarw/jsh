# JSH - Joseph's Shell

## Features

- **Core Command Execution:**
  - Supports built-in commands like `cd`, `exit`, and `help`.
  - Executes standard Unix commands with argument handling.

- **Piping (`|`):**
  - Allows combining multiple commands by connecting their input and output.

- **Aliasing:**
  - Create custom shortcuts using the `alias` command to improve productivity.

- **Configuration File:**
  - Loads user-defined aliases and settings from a `.jshrc` file.

- **Error Handling:**
  - Provides clear error messages for invalid commands or syntax issues.

## Installation

### Dependencies

Ensure the following dependencies are installed:

**GCC Compiler:**  
  - **apt**:
    ```shell
    sudo apt install build-essential
    ```
  - **Homebrew**:
    ```shell
    brew install gcc
    ```
  - **dnf**:
    ```shell
    sudo dnf install gcc
    ```
**Readline Library:**
  - **apt**:
    ```shell
    sudo apt install libreadline-dev
    ```
  - **Homebrew**:
    ```shell
    brew install readline
    ```
  - **dnf**:
    ```shell
    sudo dnf install readline-devel
    ```


### Build the Shell

Clone the repository and compile **JSH**:

```shell
git clone https://github.com/jgarw/jsh.git 
cd jsh
gcc shell.c -o jsh -lreadline
```

### Run the Shell

```shell
./jsh
```

## Usage

### Basic Commands

Run any Unix-like command:

```shell
jsh> ls -la
```

### Piping Commands

Connect commands using the `|` operator:

```shell
jsh> cat file.txt | grep "search term"
```

### Built-in Commands

- **Change Directory**:
  ```shell
  jsh> cd /path/to/directory
  ```
- **Exit the Shell**:
  ```shell
  jsh> exit
  ```

### Aliasing

Create aliases for frequently used commands:

```shell
jsh> alias ll="ls -la"
```

### Configuration with `.jshrc`

Add aliases and custom configurations in a `.jshrc` file located in your home directory:

```shell
alias ll="ls -la"
alias cat="bat"
```

## Roadmap

- **Job Control**: Background and foreground process management.
- **Redirection**: Support for input and output redirection (`>`, `>>`, `<`).
- **Scripting**: Ability to execute shell scripts.

## Contributing

Contributions are welcome! If you'd like to improve **JSH**, please open an issue or submit a pull request.