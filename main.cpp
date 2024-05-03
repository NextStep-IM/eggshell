#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include <linux/limits.h>

char **get_input(char *input)
{
    char **command = new char *[8];
    const char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator); // strtok returns pointer to next token
    while (parsed != NULL)
    {
        command[index] = parsed;
        index++;

        // NULL argument instructs compiler to use source c-string
        parsed = strtok(NULL, separator); // Gets the next token i.e. the token before second separator
    }

    command[index] = NULL;

    return command;
}

int cd(char *path)
{
    return chdir(path);
}

// argc (size of command array) is problematic.
int main()
{
    char **command;
    char *input;
    pid_t child_pid{};
    int status{};

    while (1)
    {
        char cwd[PATH_MAX];
        std::cout << "\033[30;44m" << getcwd(cwd, sizeof(cwd)) << "\033[0m\n";
        // line returned by readline is allocated with malloc
        input = readline("\033[92m-> \033[0m");
        command = get_input(input);
        int cmd_len = sizeof(command);

        // Handles empty commands
        if (!command[0])
        {
            free(input);
            delete[] command;
            continue;
        }

        // cd command
        if (strcmp(command[0], "cd") == 0)
        {
            if (cd(command[1]) < 0)
            {
                std::cerr << "Error: No such file or directory.\n";
            }
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
            // Never returns if the call is successful
            execvp(command[0], command);

            if (execvp(command[0], command) < 0)
            {
                std::cerr << "Error: Invalid input."
                          << "\n";
                return -1;
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
