#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include "utilities.h"
#include "colors.h"
namespace fs = std::filesystem;

// Function Definitions

// Parses the input from user
std::vector<std::string> getInput(std::string input)
{
    std::vector<std::string> command;
    std::istringstream iss(input);
    std::string token;

    // std::quoted removes the quotes around an argument and passes it as a whole
    while (iss >> std::quoted(token))
    {
        command.push_back(token);
    }
    return command;
}

// Changes current working directory
int cd(fs::path dirPath)
{
    std::string path = dirPath.string();
    return chdir(path.c_str());
}

// Creates files
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

// Creates directories
void crtdir(fs::path dirPath)
{
    // Maybe add error_code object?

    if (!fs::exists(dirPath))
    {
        fs::create_directories(dirPath);
        std::cout << "Directory: " << dirPath << "\n";
    }
    else
    {
        std::cout << "Directory already exists\n";
    }
}

// Reads files
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

// List directory content
void exp(fs::path target)
{
    // std::cout << "exp command working.\n";
    //  Iterates through the contents of the target directory (non-recursively)
    for (auto const dir_entry : fs::directory_iterator(target))
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

// Delete files and directories
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
}

// Copy files and directories
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

// Move files and directories
void mov(fs::path old_path, fs::path new_path)
{
    std::error_code ec;
    if (fs::is_regular_file(old_path) && fs::is_directory(new_path))
    {
        cpy(old_path, new_path);
        del(old_path);
    }
    else if (fs::is_directory(old_path) && fs::is_directory(new_path))
    {
        fs::path combinedPath = new_path / old_path.filename();
        std::cout << combinedPath << "\n";
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

// Show list of directories
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

// Checks command availability
int cmdCheck(std::vector<std::string> command)
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
                    return -1;
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
            return -1;
        }
    }

    else if (command[0] == "cd")
    {
        if (cd(command[1]) < 0)
        {
            std::cerr << "Error: No such file or directory.\n";
            return -1;
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
            return -1;
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
            return -1;
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
            // std::cout << "argument provided\n";
            for (int i = 1; i < command.size(); ++i)
            {
                // fs::path target = command.at(i);
                exp(command[i]);
            }
        }
        else
        {
            // std::cout << "no argument provided\n";
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
            return -1;
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
                    // std::cout << "i = " << i << "\ncommand.size() = " << command.size() << "\n";
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
            return -1;
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
            return -1;
        }
    }

    else if (command[0] == "grep")
    {
        if (command.size() > 1)
        {
            for (int i = 2; i < command.size(); ++i)
            {
                grep(command[1], command[i]);
            }
        }
        else
        {
            std::cerr << "Invalid Argument" << "\n"
                      << "Usage: grep [PATTERN] [FILENAME]" << "\n";
            return -1;
        }
    }

    else if (command[0] == "clean")
    {
        clean();
    }

    else
    {
        std::cerr << RED_FG << command[0] << ": Command not found" << RESET << "\n";
    }
}

void grep(std::string pattern, fs::path filePath)
{
    //std::error_code eCode;
    if (fs::exists(filePath) && fs::is_regular_file(filePath))
    {
        std::ifstream readFile(filePath);
        if (!readFile.is_open())
        {
            std::cerr << filePath << " could not be opened." << "\n";
        }

        std::string line;
        while (std::getline(readFile, line))
        {
            if (line.find(pattern) != std::string::npos)
            {
                std::cout << BOLD_YELLOW_FG << filePath.filename().string() << RESET << ":  " << line << "\n";
            }
        }
        readFile.close();
    }

void clean()
{
    system("clear");
}