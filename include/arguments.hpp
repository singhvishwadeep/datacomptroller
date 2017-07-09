
#ifndef   ARGUMENTS_H
#define   ARGUMENTS_H
#include <string>
#include <vector>
#include <map>

int ParseArguments(int argc, char** argv, std::vector <std::map<std::string,std::string>> &action);
int CheckFileExists (std::string &filename);

#endif
