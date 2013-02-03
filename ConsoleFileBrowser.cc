#include "ConsoleFileBrowser.h"
#include "fileUtils.h"

#include <curses.h>
#include <unistd.h>

#include <vector>
#define LOOP_DELAY 50000 //.05sec

ConsoleFileBrowser::ConsoleFileBrowser()
  :aLoopSleepTime(LOOP_DELAY),
   aDirectoryMode(true),
   aCurrentFileState(false),
   aRefreshFiles(true),
   lRedraw(true)
{
  // libCurses initialization

  initscr();
  start_color();
  cbreak();
  noecho();
  nonl();
  nodelay(stdscr,true); //hmmm... would like to set this to false but then the screen doesn't respond to updates
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLACK);
  init_pair(2,COLOR_GREEN,COLOR_BLACK);
  init_pair(3,COLOR_BLUE,COLOR_BLACK);
  init_pair(4,COLOR_WHITE,COLOR_BLACK);
  init_pair(5,COLOR_WHITE,COLOR_BLACK);
  init_pair(6,COLOR_RED,COLOR_BLACK);

  // File system initialization

  char lBuf[250];
  getcwd(lBuf,249);
  aCurrentDirectory = lBuf;
  aHomeDirectory = lBuf;

  aCurrentMatchNames.push_back("*");

  aNbDirectories = 0;
  aNbFiles = 0;
  aTotalFiles = 0;

  // Internal list init
  aCurrentSelection = 0;
  aListVisibleBase = 0;
  aLastSelection = "";

}

ConsoleFileBrowser::~ConsoleFileBrowser()
{
  endwin();
}

void ConsoleFileBrowser::setCurrentDirectory(const std::string& pDirectory)
{
  aCurrentDirectory = pDirectory;
}

void ConsoleFileBrowser::setCurrentMatchNames(const std::vector<std::string>& pMatchNames)
{
  aCurrentMatchNames = pMatchNames;
}

void ConsoleFileBrowser::mainLoop()
{
  int lKey = 0;


  startLoop();
  getLocalFiles();

  redraw();

  do {

    {
      bool lRedrawForIteration = loopIteration();
      if(lRedrawForIteration) {
        lRedraw = true;
      }
    }

    if(lRedraw) {
      redraw();
    }

    lKey = getch();

    switch(lKey) {
      case 13: // Enter
        enterKey();
        lRedraw = true;
        break;
      case 'd':
        aDirectoryMode = ! aDirectoryMode;
        lRedraw = true;
        break;
      case KEY_UP:
        upKey();
        lRedraw = true;
        break;
      case KEY_DOWN:
        downKey();
        lRedraw = true;
        break;
      case KEY_NPAGE:
        pageDown();
        lRedraw = true;
        break;
      case KEY_PPAGE:
        pageUp();
        lRedraw = true;
        break;
      case 'h':
        aCurrentDirectory = aHomeDirectory;
        chdir(aCurrentDirectory.c_str());
        aRefreshFiles = true;
        aListVisibleBase = 0;
        lRedraw = true;
        break;
      case 'R':
        aRefreshFiles = true;
        break;
      default:
        if(!keyEvent(lKey)) {
          lRedraw = false;
        }
        else {
          lRedraw = true;
        }
    }

    usleep(aLoopSleepTime);
  } while(lKey != 27 && lKey != 'q');

  endLoop();
}



#define aHeaderHeight 3;
#define aFooterHeight 3;


void ConsoleFileBrowser::headerCallback()
{
  aCallbackNotExecuted = true;
}



void ConsoleFileBrowser::footerCallback()
{
  aCallbackNotExecuted = true;
}

#define LINES 17
void ConsoleFileBrowser::redraw()
{
  clear();

  getLocalFiles();

  // Drawing the header...

  aCallbackNotExecuted=false;
  headerCallback();
  if(aCallbackNotExecuted) {
	 printw("---------- Blacklight Console File Browser ----------");
    printw("INDEX OF : %s\n",aCurrentDirectory.c_str());
  }

  int lMaxSize = LINES - aHeaderHeight - aFooterHeight;
  int lVisibleSize = lMaxSize;
  // Drawing the file list
  int lMax = aListVisibleBase+lVisibleSize;
  if(lMax > aTotalFiles) {
    lMax = aTotalFiles;
  }

  int lCompte=0;
  for(int i=aListVisibleBase;i<lMax;i++) {
    if(i < aNbDirectories) {
      attron(WA_BOLD);
      attron(COLOR_PAIR(2));
    }
    else {
      attron(WA_BOLD);
      attron(COLOR_PAIR(4));
    }
    if(i == aCurrentSelection) {
      printw(" > ");
    }
    else {
      if(aCurrentFileState && aCurrentFiles[i] == aCurrentFileName && aCurrentDirectory == aCurrentFileDirectory) {
        printw(" * ");
      }
      else {
        printw("   ");
      }
    }
    std::string lSubstr = aCurrentFiles[i];
    if(aCurrentFiles[i].size() > COLS-10) {
      lSubstr = aCurrentFiles[i].substr(0,COLS-10);
    }
    printw("%s\n",lSubstr.c_str());
    lCompte++;
  }
  if(lCompte < lMaxSize) {
    for(int j=lCompte;j<lMaxSize;j++) {
      printw("\n");
    }
  }

  attroff(WA_BOLD);
//  printw("\n");

  // Drawing the screen bottom
  attron(COLOR_PAIR(4));
  aCallbackNotExecuted=false;
  footerCallback();
  if(aCallbackNotExecuted) {
	 printw("-----------------------------------------------------");
     printw("     UP/DOWN: Select   ENTER: Select   q: Quit       ");
     printw("-----------------------------------------------------");
  }
}

void ConsoleFileBrowser::enterKey()
{
  if(aCurrentFiles.empty()) {
    return;
  }

  if(aCurrentSelection < aNbDirectories) {
    if(aCurrentFiles[aCurrentSelection] == "..") {
      removeLastPathPart(aCurrentDirectory);
      char lPath[1024];
      strcpy(lPath,aCurrentDirectory.c_str());
      chdir(lPath);
      aRefreshFiles = true;
      aCurrentSelection = 0;
    }
    else {
      aCurrentDirectory += "/";
      aCurrentDirectory += aCurrentFiles[aCurrentSelection];
      char lPath[1024];
      strcpy(lPath,aCurrentDirectory.c_str());
      chdir(lPath);
      aRefreshFiles = true;
      aCurrentSelection = 0;
      aListVisibleBase=0;
    }
  }
  else {
    fileSelected(aCurrentDirectory+"/"+aCurrentFiles[aCurrentSelection]);
  }
}

void ConsoleFileBrowser::pageUp()
{
  if(aCurrentFiles.empty()) {
    return;
  }

  int lMaxSize = LINES - aHeaderHeight - aFooterHeight;
  aCurrentSelection -=lMaxSize;
  if(aCurrentSelection < 0) {
    aCurrentSelection = aTotalFiles-1;
  }
  if(aCurrentSelection < aListVisibleBase) {
    aListVisibleBase = aCurrentSelection;
  }

  int lVisibleSize = lMaxSize;
  if(aListVisibleBase+lVisibleSize <= aCurrentSelection) {
    aListVisibleBase = aCurrentSelection-(lVisibleSize-1);
  }
}


void ConsoleFileBrowser::upKey()
{
  if(aCurrentFiles.empty()) {
    return;
  }

  aCurrentSelection --;
  if(aCurrentSelection < 0) {
    aCurrentSelection = aTotalFiles-1;
  }
  if(aCurrentSelection < aListVisibleBase) {
    aListVisibleBase = aCurrentSelection;
  }

  int lMaxSize = LINES - aHeaderHeight - aFooterHeight;
  int lVisibleSize = lMaxSize;
  if(aListVisibleBase+lVisibleSize <= aCurrentSelection) {
    aListVisibleBase = aCurrentSelection-(lVisibleSize-1);
  }
}


void ConsoleFileBrowser::downKey()
{
  if(aCurrentFiles.empty()) {
    return;
  }

  aCurrentSelection++;

  int lMaxSize = LINES - aHeaderHeight - aFooterHeight;
  int lVisibleSize = lMaxSize;

  if(aTotalFiles < lMaxSize) {
    lVisibleSize = aTotalFiles;
  }

  if(aTotalFiles <= aCurrentSelection) {
    aListVisibleBase = 0;
    aCurrentSelection = 0;
  }

  if(aListVisibleBase+lVisibleSize <= aCurrentSelection) {
    aListVisibleBase = aCurrentSelection-(lVisibleSize-1);

  }
}
void ConsoleFileBrowser::pageDown()
{
  if(aCurrentFiles.empty()) {
    return;
  }

  int lMaxSize = LINES - aHeaderHeight - aFooterHeight;
  aCurrentSelection+=lMaxSize;

  int lVisibleSize = lMaxSize;

  if(aTotalFiles < lMaxSize) {
    lVisibleSize = aTotalFiles;
  }

  if(aTotalFiles <= aCurrentSelection) {
    aListVisibleBase = 0;
    aCurrentSelection = 0;
  }

  if(aListVisibleBase+lVisibleSize <= aCurrentSelection) {
    aListVisibleBase = aCurrentSelection-(lVisibleSize-1);

  }
}
void ConsoleFileBrowser::setLoopSleepTime(int pTimeUSleep)
{
  aLoopSleepTime = pTimeUSleep;
}

void ConsoleFileBrowser::getLocalFiles()
{
  if(aRefreshFiles == false) {
    return;
  }
  aRefreshFiles = false;

  // Getting the directory data
  std::vector<std::string> lFiles,lDirectories;


  aCurrentFiles = getListOfFiles(aCurrentDirectory.c_str(),
                                 aCurrentMatchNames);
  aNbFiles = aCurrentFiles.size();

  if(aDirectoryMode){
    std::vector<std::string> lDirectories = getListOfSubDir(aCurrentDirectory.c_str());
    aNbDirectories = lDirectories.size();
    for(int i=aNbDirectories-1;i>=0;i--) {
      aCurrentFiles.insert(aCurrentFiles.begin(),lDirectories[i]);
    }

    //..
    aCurrentFiles.insert(aCurrentFiles.begin(),"..");
    aNbDirectories++;
  }
  else {
    aNbDirectories = 0;
  }

  aTotalFiles = aNbDirectories+aNbFiles;
}

const std::vector<std::string>& ConsoleFileBrowser::getCurrentFiles()
{
  return aCurrentFiles;
}

int ConsoleFileBrowser::getSelection()
{
  return aCurrentSelection;
}

void ConsoleFileBrowser::setSelection(int pSelection)
{
  if(pSelection > 0 && pSelection < aCurrentFiles.size()) {
    aCurrentSelection = pSelection;
    aListVisibleBase = pSelection;
  }
}

int ConsoleFileBrowser::getNbDirectories()
{
  return aNbDirectories;
}

int ConsoleFileBrowser::getTotalFiles()
{
  return aTotalFiles;
}

const std::string& ConsoleFileBrowser::getCurrentDirectory()
{
  return aCurrentDirectory;
}

void ConsoleFileBrowser::setCurrentFileIndicator(const std::string& pFileName)
{
  aCurrentFileState = true;

  int lLastSlash = pFileName.find_last_of("/");
  if(lLastSlash != std::string::npos) {
    aCurrentFileDirectory = pFileName.substr(0,lLastSlash);
    aCurrentFileName = pFileName.substr(lLastSlash+1,pFileName.size()-(lLastSlash+1));
  }
}

void ConsoleFileBrowser::disableCurrentFileIndicator()
{
  aCurrentFileState = false;
}
