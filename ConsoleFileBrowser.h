#ifndef ConsoleFileBrowser_h
#define ConsoleFileBrowser_h

#include <string>
#include <vector>

class ConsoleFileBrowser
{
public:
  ConsoleFileBrowser();
  virtual ~ConsoleFileBrowser();

  // Change the current directory (by defaut, the directory is the "current")
  void setCurrentDirectory(const std::string& pDirectory);
  void setCurrentMatchNames(const std::vector<std::string>& pVectMatchName);

  void mainLoop();

  const std::vector<std::string>& getCurrentFiles();
  int getSelection();
  void setSelection(int pSelection);
	
protected:
  virtual void fileSelected(const std::string& pFile) = 0;
  virtual bool keyEvent(int pKey) = 0;

  virtual void startLoop() { };
  virtual bool loopIteration() { return true; };
  virtual void endLoop() { };

  void setLoopSleepTime(int pTimeUSleep);


  virtual void headerCallback()=0;
  virtual void footerCallback()=0;

  int getNbDirectories();
  int getTotalFiles();
  const std::string& getCurrentDirectory();

  void setCurrentFileIndicator(const std::string& pFileName);
  void disableCurrentFileIndicator();

protected:
  void redraw();
  void getLocalFiles();

  void enterKey();
  void upKey();
  void downKey();
  void pageDown();
  void pageUp();

  int aLoopSleepTime;

  std::string aHomeDirectory;

  std::string aCurrentDirectory;
  std::vector<std::string> aCurrentMatchNames;

  bool aDirectoryMode;

  bool aRefreshFiles;
  int aCurrentSelection;
  int aListVisibleBase;
  std::string aLastSelection;
  std::vector<std::string> aCurrentFiles;
  int aNbFiles;
  int aNbDirectories;
  int aTotalFiles;
  bool lRedraw;
	int row,col;
	int curRow;
	
  bool aCurrentFileState;
  std::string aCurrentFileDirectory;
  std::string aCurrentFileName;

  bool aCallbackNotExecuted;
};

#endif //ConsoleFileBrowser_h
