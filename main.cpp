#include <iostream>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include <linux/limits.h>
namespace fs = std::filesystem;

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
int cfile(int argc, char **argv)
{
    std::cout << "cfile command working\n";
    int opt;

    // commenting this makes the noFlag block work.
    while (opt = getopt(argc, argv, "f:b:"))
    {
        switch (opt)
        {
        case 'b':
            for (int i = 2; i < argc; ++i)
            {
                std::ofstream outputFile(argv[i]);

                if (outputFile.is_open())
                {
                    outputFile << "#include <iostream>\nusing namespace std;\n\nint main() {\n\n\treturn 0;\n}"
                               << "\n";
                    outputFile.close();
                    std::cout << argv[i] << " created\n";
                }
                else
                {
                    std::cerr << "Error: " << argv[i] << " cannot be opened"
                              << "\n";
                }
            }
            break;
        case 'f':
            for (int i = 2; i < argc; ++i)
            {
                std::ofstream outputFile(argv[i]);
                if (outputFile.is_open())
                {
                    outputFile.close();
                    std::cout << argv[i] << " created\n";
                }
                else
                {
                    std::cerr << "Error: " << argv[i] << " cannot be opened"
                              << "\n";
                }
            }
            break;
        case '?':
            std::cerr << "Error: Invalid Option."
                      << "\n";
            break;
        }
    }
    return 0;
}

// Maybe add error_code object?
void crtdir(char *dir_path)
{
    if (!fs::exists(dir_path))
    {
        fs::create_directories(dir_path);
        std::cout << "Directory: " << dir_path << "\n";
    }
    else
    {
        std::cout << "Directory already exists\n";
    }
}

void paw(char *rd_file)
{
    std::cout << "Content of " << rd_file << "\n";
    std::string text;
    std::ifstream readFile(rd_file);
    while (getline(readFile, text))
    {
        std::cout << text << "\n";
    }
    readFile.close();
}

void exp(fs::path &target)
{
    // Iterates through the contents of the target directory (non-recursively)
    for (auto const &dir_entry : fs::directory_iterator(target))
    {
        if (dir_entry.is_regular_file())
        {
            std::cout << dir_entry.path().filename().string() << "\n";
        }
        else if (dir_entry.is_directory())
        {
            std::cout << "\033[1;34m" << dir_entry.path().filename().string() << "\033[0m\n";
        }
    }
}

void del(fs::path target)
{
    std::error_code eCode;
    if (fs::is_regular_file(target))
    {
        fs::remove(target);
    }
    else if (fs::is_directory(target))
    {
        fs::remove_all(target);
    }
    else
    {
        std::cout << "Error: " << target << " does not exist." << "\n";
    }
}

int main()
{
    char **command;
    char *input;
    pid_t child_pid{};
    int status{};
    bool firstRun = true;

    while (1)
    {
        if (firstRun)
        {
            std::cout << "\033[1;37mWELCOME TO EGGSHELL!\033[0m\nType \"help\" to list commands\n\n";
        }

        char cwd[PATH_MAX];
        std::cout << "\033[30;44m" << getcwd(cwd, sizeof(cwd)) << "\033[0m\n";
        // line returned by readline is allocated with malloc
        input = readline("\033[1;92m-> \033[0m");
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
            if (strcmp(command[0], "cfile") == 0)
            {
                cfile(cmd_len, command);
            }

            if (strcmp(command[0], "crtdir") == 0)
            {
                for (int i = 1; command[i] != NULL; ++i)
                {
                    crtdir(command[i]);
                }
            }

            if (strcmp(command[0], "paw") == 0)
            {
                for (int i = 1; command[i] != NULL; ++i)
                {
                    paw(command[i]);
                }
            }

            if (strcmp(command[0], "exp") == 0)
            {
                // Checks if there are arguments
                if (command[1])
                {
                    for (int i = 1; command[i] != NULL; ++i)
                    {
                        fs::path targetPath = command[i];
                        exp(targetPath);
                    }
                }
                else
                {
                    fs::path targetPath = fs::current_path();
                    exp(targetPath);
                }
            }

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

        firstRun = false;
        free(input);
        delete[] command;
    }

    return 0;
}
