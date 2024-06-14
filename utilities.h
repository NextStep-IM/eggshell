#ifndef UTILITIES_H
#define UTILITIES_H
namespace fs = std::filesystem;
// Function Prototypes

std::vector<std::string> getInput(std::string);
int changeDir(fs::path);
int createFile(fs::path);
void createDir(fs::path);
void readFileContent(fs::path);
void listDirContent(fs::path);
void deleteFile(fs::path);
void copyFile(fs::path, fs::path);
void moveFile(fs::path, fs::path);
void help();
int cmdCheck(std::vector<std::string>);
void findPattern(std::string, fs::path);
void clearScreen();

#endif
