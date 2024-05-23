#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "utilities.h"
#include "colors.h"
namespace fs = std::filesystem;

int main()
{
    std::vector<std::string> command{};
    std::string input{};
    int status{}, childPid{};
    bool firstRun {true};
    const char* prompt {"\033[1;92m→ \033[0m"};

    while (1)
    {
        if (firstRun)
        {
            std::cout << WHITE_FG << "WELCOME TO EGGSHELL!\n"
                      << RESET << "Type \"help\" to list commands\n\n";
            firstRun = false;
        }

        std::cout << BLACK_ON_BLUE << fs::current_path().string() << RESET << "\n";
    
        // line returned by readline is allocated with malloc
        char *tempInput = nullptr;
        tempInput = readline(prompt);
        if (*tempInput)
        {
            add_history(tempInput);
        }
        input = tempInput;
        free(tempInput);

        command = getInput(input);

        /* FOR DEBUGGING ONLY.
        std::cout << BOLD_YELLOW_FG << "Command: " << RESET;
        for (auto token : command)
        {
            std::cout << token << " ";
        }
        std::cout << "\n"; */

        // Handles empty commands
        if (command.empty())
        {
            continue;
        }

        childPid = fork();

        if (childPid < 0)
        {
            std::cerr << "Error: Fork failed."
                      << "\n";
            return -1;
        }

        if (childPid == 0)
        {
            cmdCheck(command);
        }
        else
        {
            // Wait for child process (command) to finish execution
            waitpid(childPid, &status, WUNTRACED);
        }
    }
    return 0;
}
