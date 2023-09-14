#include <iostream>
#include <unistd.h>
#include "shelpers.hpp"
#include <cstring>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <vector>

char **character_name_completion(const char *, int, int);

char *character_name_generator(const char *, int);

// add a list of commands
char *customCommands[] = {(char *) "cd", (char *) "echo", (char *) "head", (char *) "cat", (char *) "touch",
                          (char *) "ls", NULL};

int main(int argc, char *argv[]) {
    rl_attempted_completion_function = character_name_completion;
    rl_bind_key('\t', rl_complete); //enable tab completion
    char *line;
    // loop to read line by line from user input
    while ((line = readline("> ")) != nullptr) {

        // handle the case when no command was provided
        std::string temp(line);
        if (temp.empty()) {
            std::cout << "Invalid command" << std::endl;
            continue;
        }

        if (temp == "exit") {
            break;
        }
        if (line[0] != '\0') {
            add_history(line); // add input to the history
        }

        std::vector<std::string> tokens = tokenize(line);
        std::vector<Command> commands = getCommands(tokens); // get the commands

        if (commands[0].exec == "cd") {
            const char *directory = commands[0].argv[1];
            // if nothing is provided, direct it to home
            if (directory == nullptr) {
                if (chdir(getenv("HOME")) != 0) {
                    perror("error in directing to HOME");
                }

            } else {
                // direct it to the arg on the left
                int check = chdir(commands[0].argv[1]);
                if (check < 0) {
                    perror("Error in chdir() ");
                }
            }
        } else {
            for (Command cmd: commands) { // for each command, create a fork
                pid_t pid = fork();
                if (pid == 0) {
                    // if there is input redirection, update the standard input file descriptor
                    if (cmd.fdStdin != 0) {
                        if (dup2(cmd.fdStdin, STDIN_FILENO) == -1) { // make FdStdin the new standard input
                            perror("dup2() Error on stdin");
                        }
                    }
                    // if there is output redirection, update the standard output file descriptor
                    if (cmd.fdStdout != 1) {
                        if (dup2(cmd.fdStdout, STDOUT_FILENO) == -1) { // make FdStout the new standard output
                            perror("dup2() Error on stdout");
                        }
                    }
                    // arguments for the commands
                    if ((execvp(cmd.exec.c_str(), const_cast<char *const *>(cmd.argv.data()))) == -1) {
                        perror("Error executing cmd ");
                    }
                } else if (pid > 0) {
                    if (cmd.fdStdout != 1) {
                        close(cmd.fdStdout);
                    }
                    if (cmd.fdStdin != 0) {
                        close(cmd.fdStdin);
                    }
                    // parent waiting for the child
                    int id_wait;
                    if (waitpid(pid, &id_wait, 0) == -1) { // wait for the child process to terminate
                        perror("wait pid error");
                    }

                } else {
                    // Error handling for fork
                    perror("Fork error");
                }
            }
        }
    }

    return 0;
}

char **
character_name_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 0;
    return rl_completion_matches(text, character_name_generator);
}

char *
character_name_generator(const char *text, int state) {
    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = customCommands[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return nullptr;
}

