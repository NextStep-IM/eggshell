#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>

char **get_input(char *input)
{
    char **command = new char *[8]; // TODO: Check if alloacted space for char pointers should be increased/decreased
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

    // Test to check tokens stored in command array

    /*std::cout << "Tokens passed to command array: ";
    for (int i = 0; i < index; ++i)
    {
        std::cout << command[i] << " ";
    }*/

    return command;
}

int main()
{
    char **command;
    char *input;
    pid_t child_pid;
    int status;

    while (1)
    {
        // Time
        // TODO: Only show time (not month, day, etc.)
        time_t curr_time = time(0);
        char *date_time = ctime(&curr_time);
        std::cout.width(185); std::cout << std::right << "\033[92m" << date_time; 

        input = readline("\033[92meggshell-> \033[0m"); // \033[92m = light green, \033[0m = default
        command = get_input(input);

        child_pid = fork();

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

        else if (child_pid < 0)
        {
            std::cerr << "Error: Fork failed.";
            return -1;
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