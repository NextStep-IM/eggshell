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
int cfile(char *fileName)
{
    std::ofstream outputFile(fileName);
    if (outputFile.is_open())
    {
        outputFile.close();
        return 1;
    }
    else
    {
        return -1;
    }
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

// Issue: can't copy directory as a whole, only its contents and sub-directories
void cpy(fs::path &src, fs::path &dest)
{
    std::error_code ec;
    if (fs::is_regular_file(src))
    {
        fs::copy(src, dest, fs::copy_options::skip_existing, ec);
        if (ec)
        {
            std::cerr << "Error copying file: " << ec.message() << "\n";
        }
        else
        {
            std::cout << "Success: File copied to " << dest << "\n";
        }
    }
    else if (fs::is_directory(src))
    {
        fs::copy(src, dest, fs::copy_options::recursive | fs::copy_options::skip_existing, ec);
        if (ec)
        {
            std::cerr << "Error copying directory: " << ec.message() << "\n";
        }
        else
        {
            std::cout << "Success: Directory copied to " << dest << "\n";
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

int main()
{
    char **command, *input;
    int status, child_pid;
    bool firstRun = true;

    while (1)
    {
        if (firstRun)
        {
            std::cout << "\033[1;37mWELCOME TO EGGSHELL!\033[0m\nType \"help\" to list commands\n\n";
            firstRun = false;
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
                }
            }

            else if (strcmp(command[0], "crtdir") == 0)
            {
                for (int i = 1; command[i] != NULL; ++i)
                {
                    crtdir(command[i]);
                }
            }

            else if (strcmp(command[0], "paw") == 0)
            {
                for (int i = 1; command[i] != NULL; ++i)
                {
                    paw(command[i]);
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
                for (int i = 1; command[i] != NULL; ++i)
                {
                    del(command[i]);
                }
            }

            else if (strcmp(command[0], "cpy") == 0)
            {
                fs::path dest;
                int i;
                for (i = 1; command[i] != NULL; ++i)
                {
                    if (command[i + 1] == NULL)
                    {
                        dest = command[i];
                        std::cout << "Destination: " << dest << "\n";
                        break;
                    }
                }
                for (int j = 1; j < i; ++j)
                {
                    fs::path src = command[j];
                    std::cout << "Source file/directory: " << src << "\n";
                    cpy(src, dest);
                }
            }

            else if (strcmp(command[0], "help") == 0)
            {
                help();
            }

            else if (strcmp(command[0], "mov") == 0)
            {
                fs::path new_path;
                int i;
                // to find new path/last argument
                for (i = 1; command[i] != NULL; ++i)
                {
                    if (command[i + 1] == NULL)
                    {
                        new_path = command[i];
                        break;
                    }
                }

                for (int j = 1; j < i; ++j)
                {
                    mov(command[j], new_path);
                }
            }

            else
            {
                std::cerr << "\033[31m" << command[0] << ": Command not found" << "\033[0m\n";
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
