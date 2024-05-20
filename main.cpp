#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include <climits>
#include "utilities.h"
#include "colors.h"
namespace fs = std::filesystem;

int main()
{
    std::vector<std::string> command;
    std::string input;
    int status{}, child_pid{};
    bool firstRun = true;

    while (1)
    {
        if (firstRun)
        {
            std::cout << WHITE_FG << "WELCOME TO EGGSHELL!\n"
                      << RESET << "Type \"help\" to list commands\n\n";
            firstRun = false;
        }

        char cwd[PATH_MAX];
        std::cout << BLACK_ON_BLUE << getcwd(cwd, sizeof(cwd)) << RESET << "\n";

        // line returned by readline is allocated with malloc
        char *temp_input = nullptr;
        temp_input = readline("\033[1;92m-> \033[0m");
        input = temp_input;
        free(temp_input);

        command = get_input(input);

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

        child_pid = fork();

        if (child_pid < 0)
        {
            std::cerr << "Error: Fork failed."
                      << "\n";
            return -1;
        }

        if (child_pid == 0)
        {
            if (command[0] == "cfile")
            {
                if (command.size() > 1)
                {
                    for (int i = 1; i < command.size(); ++i)
                    {
                        if (cfile(command[i]) < 0)
                        {
                            std::cerr << "Error: " << command[i] << " cannot be opened" << "\n";
                        }
                        else
                        {
                            std::cout << command[i] << " created\n";
                        }
                    }
                }
                else
                {
                    std::cerr << "Error: missing file name" << "\n";
                    break;
                }
            }

            // change directory command
            else if (command[0] == "cd")
            {
                if (cd(command[1]) < 0)
                {
                    std::cerr << "Error: No such file or directory.\n";
                }
                // continue;
            }

            else if (command[0] == "crtdir")
            {
                if (command.size() > 1)
                {
                    for (int i = 1; i < command.size(); ++i)
                    {
                        crtdir(command[i]);
                    }
                }
                else
                {
                    std::cerr << "Error: missing file name" << "\n";
                    break;
                }
            }

            else if (command[0] == "paw")
            {
                if (command.size() > 1)
                {
                    for (int i = 1; i < command.size(); ++i)
                    {
                        paw(command[i]);
                    }
                }
                else
                {
                    std::cerr << "Error: missing file name" << "\n";
                    break;
                }
            }

            else if (command[0] == "exp")
            {
                /* FOR DEBUGGING ONLY.
                            std::cout << "command[0] == exp\n";
                            std::cout << command[1] << "\n";
                            std::cout << command.size() << "\n";
                */

                // Checks if there are arguments
                if (command.size() > 1)
                {
                    //std::cout << "argument provided\n";
                    for (int i = 1; i < command.size(); ++i)
                    {
                        // fs::path target = command.at(i);
                        exp(command[i]);
                    }
                }
                else
                {
                    //std::cout << "no argument provided\n";
                    fs::path targetPath = fs::current_path();
                    exp(targetPath);
                }
            }

            else if (command[0] == "del")
            {
                if (command.size() > 1)
                {
                    for (int i = 1; i < command.size(); ++i)
                    {
                        del(command[i]);
                    }
                }
                else
                {
                    std::cerr << "Error: missing file name" << "\n";
                    break;
                }
            }

            else if (command[0] == "cpy")
            {
                if (command.size() > 1)
                {
                    int i;
                    for (i = 1; i < command.size(); ++i)
                    {
                        if (i + 1 == command.size())
                        {
                            //std::cout << "i = " << i << "\ncommand.size() = " << command.size() << "\n";
                            break;
                        }
                    }
                    for (int j = 1; j < i; ++j)
                    {
                        cpy(command[j], command[i]);
                    }
                }
                else
                {
                    std::cerr << "Error: missing file name" << "\n";
                    break;
                }
            }

            else if (command[0] == "help")
            {
                help();
            }

            else if (command[0] == "mov")
            {
                if (command.size() > 1)
                {
                    int i;
                    // to find new path/last argument
                    for (i = 1; i < command.size(); ++i)
                    {
                        if (i + 1 == command.size())
                        {
                            break;
                        }
                    }
                    for (int j = 1; j < i; ++j)
                    {
                        mov(command[j], command[i]);
                    }
                }
                else
                {
                    std::cerr << "Error: missing file name" << "\n";
                    break;
                }
            }

            else
            {
                std::cerr << RED_FG << command[0] << ": Command not found" << RESET << "\n";
            }
        }

        else
        {
            // Wait for child process (command) to finish execution
            waitpid(child_pid, &status, WUNTRACED);
        }

    }

    return 0;
}
