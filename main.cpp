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

// Color Codes

const std::string RESET = "\033[0m",
                  WHITE_FG = "\033[1;37m",
                  BLACK_ON_BLUE = "\033[30;44m",
                  RED_FG = "\033[31m",
                  BOLD_BLUE_FG = "\033[1;34m",
                  BOLD_YELLOW_FG = "\033[1;33m";

// Function Prototypes

char **get_input(char *input);
int cd(char *path);
int cfile(fs::path fileName);
void crtdir(fs::path dir_path);
void paw(fs::path readFile);
void exp(fs::path target);
void del(fs::path target);
void cpy(fs::path src, fs::path dest);
void mov(fs::path old_path, fs::path new_path);
void help();

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

// Function Definitions

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
int cfile(fs::path fileName)
{
    std::ofstream outputFile(fileName);
    if (outputFile.is_open())
    {
        outputFile.close();
        return 0;
    }
    else
    {
        return -1;
    }
}

// Maybe add error_code object?
void crtdir(fs::path dir_path)
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

void paw(fs::path readFile)
{
    if (!fs::exists(readFile))
    {
        std::cerr << "Error: " << readFile << " does not exist." << "\n";
        return;
    }
    std::cout << BOLD_YELLOW_FG << readFile << RESET << "\n";
    std::string text;
    std::ifstream File(readFile);
    int line = 1;
    while (getline(File, text))
    {
        std::cout << line << "| " << text << "\n";
        ++line;
    }
    File.close();
    std::cout << "\n";
}

void exp(fs::path target)
{
    // Iterates through the contents of the target directory (non-recursively)
    for (auto dir_entry : fs::directory_iterator(target))
    {
        if (dir_entry.is_regular_file())
        {
            std::cout << dir_entry.path().filename().string() << "\n";
        }
        else if (dir_entry.is_directory())
        {
            std::cout << BOLD_BLUE_FG << dir_entry.path().filename().string() << RESET << "\n";
        }
    }
}

void del(fs::path target)
{
    std::error_code eCode;

    if (!fs::exists(target))
    {
        std::cerr << "Error: " << target << " does not exist." << "\n";
        return;
    }

    if (fs::is_regular_file(target))
    {
        fs::remove(target, eCode);
        if (eCode)
        {
            std::cerr << "Error: " << eCode.message() << "\n";
            return;
        }
    }
    else if (fs::is_directory(target))
    {
        fs::remove_all(target, eCode);
        if (eCode)
        {
            std::cerr << "Error: " << eCode.message() << "\n";
            return;
        }
    }
    /* else
    {
        std::cerr << "Error: " << target << " does not exist." << "\n";
    } */
}

// Issue: can't copy directory as a whole, only its contents and sub-directories
void cpy(fs::path src, fs::path dest)
{
    if (!fs::exists(dest))
    {
        std::cerr << "Error: " << dest << " does not exist." << "\n";
        return;
    }
    if (!fs::exists(src))
    {
        std::cerr << "Error: " << src << " does not exist." << "\n";
        return;
    }
    std::error_code eCode;
    if (fs::is_regular_file(src))
    {
        fs::copy(src, dest, eCode);
        if (eCode)
        {
            std::cerr << "Error: " << eCode.message() << "\n";
            return;
        }

    }
    else if (fs::is_directory(src))
    {
        fs::copy(src, dest, fs::copy_options::recursive, eCode);
        if (eCode)
        {
            std::cerr << "Error: " << eCode.message() << "\n";
            return;
        }
    }
}

void mov(fs::path old_path, fs::path new_path)
{
    std::error_code ec;
    if (fs::is_regular_file(old_path) && fs::is_directory(new_path))
    {
        cpy(old_path, new_path);
        del(old_path);
    }
    else if (fs::is_directory(old_path) && fs::is_directory(new_path) && fs::exists(new_path))
    {
        fs::path combinedPath = new_path / old_path.filename();
        fs::create_directory(combinedPath);
        cpy(old_path, combinedPath);
        del(old_path);
    }
    else
    {
        fs::rename(old_path, new_path, ec);
        if (ec)
        {
            std::cerr << ec.message() << "\n";
        }
        else
        {
            std::cout << old_path.filename() << " renamed to " << new_path.filename() << "\n";
        }
    }
}

void help()
{
    std::cout << "exp    -    List directory content\n"
              << "paw    -    Display file content\n"
              << "cpy    -    Copy files and directories\n"
              << "crtdir -    Create directories\n"
              << "cfile  -    Create files\n"
              << "cd     -    Change directory\n"
              << "del    -    Delete files and directories\n"
              << "mov    -    Move files and directories\n";
}