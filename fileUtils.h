#ifndef fileUtils_h
#define fileUtils_h


#include <vector>
#include <cstring>
#include <string>

std::vector<std::string> getListOfFiles(const char *pDirectory,const std::vector<std::string>& pMatchNameList);
std::vector<std::string> getListOfFiles(const char *pDirectory,const char *pPattern);
std::vector<std::string> getListOfSubDir(const char *pDirectory);
void removeLastPathPart(std::string& pPath);

#endif // fileUtils_h
