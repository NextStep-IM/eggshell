#ifndef UTILITIES_H
#define UTILITIES_H
namespace fs = std::filesystem;
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

#endif
