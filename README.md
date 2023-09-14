# Unix Shell Project

Owner: Gloria Dukuzeyesu

## Overview

In this project, I crafted a simple Unix shell in C++. This custom shell allows users to run commands, incorporating features like I/O redirection and piping, akin to common Unix shells such as bash and zsh.

## Video Walkthrough

![walkDemo](UnixShell/test_folder/walkDemo.gif)



## Running the Program

1. **Download the Code or fork the repo**

   ```
   git clone https://github.com/your-username/UnixShell.git
   ```

2. **Build the Program:**

   ```
   make
   ```

3. **Execute the Program:**

   ```
   ./program
   ```

## Test Commands

You can use the following test commands to explore the functionality of the Unix shell. In your terminal. 

- **Change Directory (`cd`):**

  - Use the `cd test_folder` command to navigate into the "test_folder" directory.
  - Use `cd ..` to move back up one directory.

- **List Contents (`ls`):**

  - Execute `ls` to display the list of contents in the current directory.

- **File Creation and Output (`echo`):**

  - Create a file named "someFile" and write the sentence "hello world" into it using the command:

    ```
    echo hello world > someFile
    ```

- **Display First Lines (`head`):**

  - Display the first 10 lines from a file called "someFile" using:

    ```
    head < someFile
    ```

- **Display Content and Filtering (`cat`, `grep`, `tail`):**

  - Use `cat file.txt` to display the contents of a file named "file.txt."

  - Filter the content to display the last 10 lines where the word "love" appears using:

    ```
    cat file.txt | grep love | tail
    ```

## What I Did

1. **Running a Single Command**: I began by creating a shell that interprets a line of input as a single user command. Using fork(), exec(), and wait(), I executed user commands and awaited their completion before processing the next input.
2. **I/O Redirection**: I expanded the shell to support I/O redirection, enabling programs to utilize files for stdin and stdout, all without their knowledge. This feature involved manipulating file descriptors using the dup2() system call, enhancing the shell's functionality.
3. **Pipes**: To enhance usability, I introduced support for piping multiple commands together. This feature empowers users to chain commands with pipes, offering capabilities similar to advanced Unix shells.
4. **Shell Builtins**: I implemented a crucial shell builtin: 'cd.' This feature allows users to change directories within the shell. 'cd' with no parameters navigates to the user's home directory, while 'cd' with a specified directory parameter allows for quick directory changes.
5. **Tab Completion**: I enhanced user experience by incorporating tab completion using the readline library. This feature simplifies command and filename input by providing auto-completion suggestions.

## Technologies Used

I utilized C++ to develop this Unix shell. This project provides a fundamental understanding of shell functionality, command execution, and input/output redirection, serving as a valuable addition to my programming skills.

Feel free to explore the code and run the shell to experience its functionality.

Thanks for visiting my Unix shell project .😊

Fell Free to connect on [Linkedin](https://www.linkedin.com/in/dgloria/)





