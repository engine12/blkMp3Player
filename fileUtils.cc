#include "fileUtils.h"
#include <algorithm>
#include <cstdio>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <set>

std::vector<std::string> getListOfFiles(const char *pDirectory,const std::vector<std::string>& pMatchNameList)
{
  std::set<std::string> lSetMatchNames;
  int lNbMatchNames=pMatchNameList.size();
  for(int i=0;i<lNbMatchNames;i++) {
    std::vector<std::string> lLocalMatch = getListOfFiles(pDirectory,pMatchNameList[i].c_str());
    int lNbLocal = lLocalMatch.size();
    for(int i=0;i<lNbLocal;i++) {
      lSetMatchNames.insert(lLocalMatch[i]);
    }
  }

  //(auto sorted)

  std::vector<std::string> lLstResult;
  std::set<std::string>::iterator lIterSet = lSetMatchNames.begin();
  const std::set<std::string>::iterator lIterSetFin = lSetMatchNames.end();
  while(lIterSet != lIterSetFin) {
    if(*lIterSet != "." && *lIterSet != "..") {
      lLstResult.push_back(*lIterSet);
    }
    ++lIterSet;
  }
  return lLstResult;
}

std::vector<std::string> getListOfFiles(const char *pDirectory,const char *pPattern)
{
  DIR *dp = 0;
  struct dirent  *dirp;

  std::vector<std::string> lstNomsFichier;

  char lPath[1024];
  strcpy(lPath,pDirectory);
  dp = opendir(lPath);
  if ( dp != 0)
  {
    while( (dirp = readdir(dp)) != NULL )
    {
      if(!fnmatch(pPattern,dirp->d_name,0))
      {
        std::string lFileName = dirp->d_name;
        if(lFileName != "." && lFileName != "..") {
          lstNomsFichier.push_back(lFileName);
        }
      }
    }
    closedir(dp);
  }
  else
    exit(2);

  std::sort(lstNomsFichier.begin(),lstNomsFichier.end());
  return lstNomsFichier;
}


std::vector<std::string> getListOfSubDir(const char *pDirectory) {
  DIR *dp;
  struct dirent  *dirp;

  std::vector<std::string> lstNomsFichier;

  if ( (dp = opendir(pDirectory)) != NULL)
  {
    while( (dirp = readdir(dp)) != NULL )
    {
        struct stat lStat;
        stat(dirp->d_name,&lStat);

        if(S_ISDIR(lStat.st_mode)) {
          std::string lFileName = dirp->d_name;
          if(lFileName != "." && lFileName != ".." && lFileName[0] != '_' && lFileName[0] != '.') {
            lstNomsFichier.push_back(lFileName);
          }
        }
    }
    closedir(dp);
  }
  std::sort(lstNomsFichier.begin(),lstNomsFichier.end());
  return lstNomsFichier;
}

void removeLastPathPart(std::string& pPath)
{
  int lPos = pPath.find_last_of("//");
  if(lPos != std::string::npos) {
    pPath = pPath.substr(0,lPos);
  }
}
