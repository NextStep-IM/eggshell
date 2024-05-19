#include <iostream>
#include <cstring>
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
    char **command, *input;
    int status, child_pid;
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
        input = readline("\033[1;92m-> \033[0m");
        command = get_input(input);

        // Handles empty commands
        if (!command[0])
        {
            free(input);
            delete[] command;
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
            if (strcmp(command[0], "cfile") == 0)
            {
                if (command[1])
                {
                    for (int i = 1; command[i] != NULL; ++i)
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
            else if (strcmp(command[0], "cd") == 0)
            {
                if (cd(command[1]) < 0)
                {
                    std::cerr << "Error: No such file or directory.\n";
                }
                continue;
            }

            else if (strcmp(command[0], "crtdir") == 0)
            {
                if (command[1])
                {
                    for (int i = 1; command[i] != NULL; ++i)
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

            else if (strcmp(command[0], "paw") == 0)
            {
                if (command[1])
                {
                    for (int i = 1; command[i] != NULL; ++i)
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

            else if (strcmp(command[0], "exp") == 0)
            {
                // Checks if there are arguments
                if (command[1])
                {
                    for (int i = 1; command[i] != NULL; ++i)
                    {
                        exp(command[i]);
                    }
                }
                else
                {
                    fs::path targetPath = fs::current_path();
                    exp(targetPath);
                }
            }

            else if (strcmp(command[0], "del") == 0)
            {
                if (command[1])
                {
                    for (int i = 1; command[i] != NULL; ++i)
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

            else if (strcmp(command[0], "cpy") == 0)
            {
                if (command[1])
                {
                    int i;
                    for (i = 1; command[i] != NULL; ++i)
                    {
                        if (command[i + 1] == NULL)
                        {
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

            else if (strcmp(command[0], "help") == 0)
            {
                help();
            }

            else if (strcmp(command[0], "mov") == 0)
            {
                if (command[1])
                {
                    int i;
                    // to find new path/last argument
                    for (i = 1; command[i] != NULL; ++i)
                    {
                        if (command[i + 1] == NULL)
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

        free(input);
        delete[] command;
    }

    return 0;
}

