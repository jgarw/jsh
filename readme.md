# Custom Command-Line Shell  

This project is a simple custom command-line shell written in C. It replicates basic shell functionalities such as navigating directories (`cd`), listing directory contents (`ls`), and accepting user commands.  

## Features  
- **Directory Navigation (`cd`)**:  
  - Change to a specified directory.  
  - Navigate to the user's home directory when no argument is provided.  
- **List Directory Contents (`ls`)**:  
  - Display the contents of the current or specified directory.  
- **Command Prompt**:  
  - Displays the current working directory (`cwd`) as the shell prompt.  
- **Input Parsing**:  
  - Processes user commands and arguments efficiently.  
- **Error Handling**:  
  - Graceful error messages for invalid commands or paths.  

## How It Works  
1. The program displays the current working directory as a prompt.  
2. Users can input commands like `ls`, `cd <path>`, or `exit`.  
3. The shell processes the input and executes the corresponding functionality.  

## Commands  
- `ls`  
  - Lists the contents of the current directory if no argument is provided.  
  - Accepts a path argument to list the contents of a specific directory.  
- `cd`  
  - Changes to the specified directory.  
  - Navigates to the user's home directory if no argument is provided.  
- `exit` or `quit`  
  - Exits the shell.  

## Prerequisites  
- A C compiler (e.g., `gcc`).  
- A Unix-like operating system (Linux/MacOS).  

## Compilation and Usage  
1. Clone this repository:  
   ```bash  
   git clone <repository_url>  
   cd <repository_name>  

