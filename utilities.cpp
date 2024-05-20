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

std::vector<std::string> getInput(std::string input)
{
    std::vector<std::string> command;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token)
    {
        command.push_back(token);
    }
    return command;
}

int cd(fs::path dir_path)
{
    std::string path = dir_path.string();
    return chdir(path.c_str());
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

// Function: Check command availability
void cmdCheck(std::vector<std::string> command)
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
            return;
        }
    }

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
            return;
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
            return;
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
            return;
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
            return;
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
            return;
        }
    }
    
    else
    {
        std::cerr << RED_FG << command[0] << ": Command not found" << RESET << "\n";
    }
}