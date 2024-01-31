#include "shelpers.hpp"

/*
  text handling functions
 */

bool splitOnSymbol(std::vector<std::string> &words, int i, char c) {
    if (words[i].size() < 2) { return false; }
    int pos;
    if ((pos = words[i].find(c)) != std::string::npos) {
        if (pos == 0) {
            //starts with symbol
            words.insert(words.begin() + i + 1, words[i].substr(1, words[i].size() - 1));
            words[i] = words[i].substr(0, 1);
        } else {
            //symbol in middle or end
            words.insert(words.begin() + i + 1, std::string{c});
            std::string after = words[i].substr(pos + 1, words[i].size() - pos - 1);
            if (!after.empty()) {
                words.insert(words.begin() + i + 2, after);
            }
            words[i] = words[i].substr(0, pos);
        }
        return true;
    } else {
        return false;
    }

}

std::vector<std::string> tokenize(const std::string &s) {

    std::vector<std::string> ret;
    int pos = 0;
    int space;
    //split on spaces
    while ((space = s.find(' ', pos)) != std::string::npos) {
        std::string word = s.substr(pos, space - pos);
        if (!word.empty()) {
            ret.push_back(word);
        }
        pos = space + 1;
    }

    std::string lastWord = s.substr(pos, s.size() - pos);
    if (!lastWord.empty()) {
        ret.push_back(lastWord);
    }

    for (int i = 0; i < ret.size(); ++i) {
        for (auto c: {'&', '<', '>', '|'}) {
            if (splitOnSymbol(ret, i, c)) {
                --i;
                break;
            }
        }
    }

    return ret;

}


std::ostream &operator<<(std::ostream &outs, const Command &c) {
    outs << c.exec << " argv: ";
    for (const auto &arg: c.argv) { if (arg) { outs << arg << ' '; }}
    outs << "fds: " << c.fdStdin << ' ' << c.fdStdout << ' ' << (c.background ? "background" : "");
    return outs;
}

//returns an empty vector on error
/*

  You'll need to fill in a few gaps in this function and add appropriate error handling
  at the end.

    //I/O redirection

                      Eventually you'll need to fill this in to support I/O Redirection
                      Note, that only the FIRST command can take input redirection
                      (all others get input from a pipe)
                      Only the LAST command can have output redirection!


*/

std::vector<Command> getCommands(const std::vector<std::string> &tokens) {
    std::vector<Command> ret(std::count(tokens.begin(), tokens.end(), "|") + 1);  //1 + num |'s commands

    int first = 0;
    int last = std::find(tokens.begin(), tokens.end(), "|") - tokens.begin(); // the index of the first
    // occurrence of | in the token vector.??
    bool error = false;
    // declare the fd for the files
    //int fd1 = 0, fd2 = 0;

    for (int i = 0; i < ret.size(); ++i) {
        if( (tokens[first] == "&") || (tokens[first] == "<") || (tokens[first] == ">") || (tokens[first] == "|") ) {
            error = true;
            break;
        }

        ret[i].exec = tokens[first]; //the name of the executable
        ret[i].argv.push_back(tokens[first].c_str()); //argv[0] = program name
        std::cout << "exec start: " << ret[i].exec << std::endl;
        ret[i].fdStdin = 0; // stdin
        ret[i].fdStdout = 1; // stdout
        ret[i].background = false;

        for (int j = first + 1; j < last; ++j) {
            //TODO I/O redirection
            if (tokens[j] == ">" || tokens[j] == "<") {
                if (j + 1 >= last) { // in case there is no file name specified after > or < , that is an error, break
                    error = true;
                    break;
                }

                if (tokens[j] == "<") {
                    if (i != 0) {
                        error = true;
                        break;
                    }
                    ret[i].fdStdin = open(tokens[j + 1].c_str(), O_RDONLY); // if i = 0, create a file descriptor with read only flag
                    if (ret[i].fdStdin == -1) { // open error check
                        perror("Error with open syscall");
                        error = true;
                        break;
                    }
                } else {
                    if (i != ret.size() - 1) {
                        error = true;
                        break;
                    }
                    ret[i].fdStdout = open(tokens[j + 1].c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
                    if (ret[i].fdStdout == -1) { // open error check
                        perror("Error with open() syscall");
                        error = true;
                        break;
                    }

                }
                j++; // skip the filename argument as it has been processed
                // assert(false); // TODO find out why asserting here ??
            } else if (tokens[j] == "&") {
                //Fill this in if you choose to do the optional "background command" part
                assert(false);
            } else {
                //otherwise this is a normal command line argument!
                ret[i].argv.push_back(tokens[j].c_str());
            }

        }


        // TODO handles pipes
        if (i > 0) {
            /* there are multiple commands.  Open a pipe and
               Connect the ends to the fds for the commands!
            */
            //pipes enables the output of each process to be directly used as input to the next one.
            // | denotes pipe
            int pipeFd[2];
            if (pipe(pipeFd) == -1) {
                perror("Error in creating pipe");
                error = true;
                break;
            }

            ret[i - 1].fdStdout = pipeFd[1];
            ret[i].fdStdin = pipeFd[0];

        }
        //exec wants argv to have a nullptr at the end!
        ret[i].argv.push_back(nullptr);
        //find the next pipe character
        first = last + 1;
        if (first < tokens.size()) {
            last = std::find(tokens.begin() + first, tokens.end(), "|") - tokens.begin();
        }
    }

    if (error) {
        // closing the fd
        for (auto &i: ret) {
            if (i.fdStdout != 1) {
                if (close(i.fdStdout) == -1) {
                    perror("Error in closing fdStdout");
                }
            }
            if (i.fdStdin != 0) {
                if (close(i.fdStdin) == -1) {
                    perror("Error in the fdStdin");
                }
            }
        }
    }
    return ret;
}
