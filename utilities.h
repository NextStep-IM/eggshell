#ifndef UTILITIES_H
#define UTILITIES_H
namespace fs = std::filesystem;
// Function Prototypes

std::vector<std::string> getInput(std::string);
int cd(fs::path);
int cfile(fs::path);
void crtdir(fs::path);
void paw(fs::path);
void exp(fs::path);
void del(fs::path);
void cpy(fs::path, fs::path);
void mov(fs::path, fs::path);
void help();
void cmdCheck(std::vector<std::string>);
void grep(std::string, fs::path);

#endif
