/*	 Copyright 2012 Mark Majeres  mark@engine12.com		*/

#include "Mp3ConsoleMenu.h"
#include "curses.h"
#include <cstdlib>

#include <linux/input.h>

Mp3ConsoleMenu& thePlayList = Mp3ConsoleMenu::getInstance();
Mp3ConsoleMenu Mp3ConsoleMenu::_instance;

Mp3ConsoleMenu::Mp3ConsoleMenu()
{
  std::vector<std::string> lVectValidMatchNames;
  lVectValidMatchNames.push_back("*.mp3");
  lVectValidMatchNames.push_back("*.MP3");
//  lVectValidMatchNames.push_back("*.m3u");
//  lVectValidMatchNames.push_back("*.M3U");
  setCurrentMatchNames(lVectValidMatchNames);
  
  aNowPlaying = false;

  aRepeat = false;
  aShuffle = false;
}

Mp3ConsoleMenu::~Mp3ConsoleMenu()
{

}

void Mp3ConsoleMenu::onNext()
{
	// SIGUSR1 handler 
	downKey();
	enterKey();
	redraw();
}

void Mp3ConsoleMenu::onPrev()
{
	// SIGUSR2 handler 
	upKey();
	enterKey();
	redraw();
}
void Mp3ConsoleMenu::fileSelected(const std::string& pFile)
{
  playFile(pFile);
  aNowPlaying = true;
}

bool Mp3ConsoleMenu::keyEvent(int pKey)
{
/*	Help with finding the keystrokes
	char strkey[100];
	snprintf(strkey, sizeof(strkey), "%d", pKey); 
	std::string command = "echo \"" + std::string(strkey) + "\" >/tmp/shitkey";
	system(command.c_str());
*/	
	bool  bRet = true;
	switch( pKey) {
	
	case 362://ZipIt Prev key
		onPrev();
		 bRet = false;
		break;

	case 385://ZipIt Next Key
		onNext();
		 bRet = false;
		break;
	
	case 's':
		aShuffle = !aShuffle;
		if(aShuffle)  
			init_pair(5, COLOR_RED, COLOR_BLACK);
		else
			init_pair(5, COLOR_WHITE, COLOR_BLACK);
		break;
  
  case 'r':
		aRepeat = !aRepeat; 
		if(aRepeat)  
			init_pair(1, COLOR_RED, COLOR_BLACK);
		else
			init_pair(1, COLOR_WHITE, COLOR_BLACK);
	
		break;
	
  default:
    bRet = false;
  }
  return bRet;
}

void Mp3ConsoleMenu::headerCallback()
{
  attron(COLOR_PAIR(3));
  attron(WA_DIM);
  printw("--------------- Blacklight MP3 player ---------------");
  attroff(WA_DIM);
  attron(COLOR_PAIR(4));
  printw(" Directory: %.40s \n",getCurrentDirectory().c_str());
}

void Mp3ConsoleMenu::footerCallback()
{
	attron(WA_DIM);
	attron(COLOR_PAIR(3));
	printw("-----------------------------------------------------");
	printw(" UP/DOWN: ");
	attron(COLOR_PAIR(4));
	printw("Select");
	attron(COLOR_PAIR(3));
	printw("     ENTER: ");
	attron(COLOR_PAIR(4));
	printw("Play");
	attron(COLOR_PAIR(3));
	printw("    Stop: ");
	attron(COLOR_PAIR(4));
	printw("STOP             ");
	attron(COLOR_PAIR(3));
	printw(" r: ");
	attron(COLOR_PAIR(1));
	printw("Repeat");
	attron(COLOR_PAIR(3));
	printw("     s: ");
	attron(COLOR_PAIR(5));
	printw("Shuffle");
	attron(COLOR_PAIR(3));	
	printw("     q: ");
	attron(COLOR_PAIR(4));
	printw("Quit          ");
	attron(COLOR_PAIR(3));
  printw("-----------------------------------------------------");

  attroff(WA_DIM);
}

bool Mp3ConsoleMenu::loopIteration()
{
  if(aNowPlaying) {

    const std::vector<std::string>& lFilesRef = getCurrentFiles();

    aNowPlaying = true;
    if(aRepeat) {
      playFile(getCurrentDirectory()+"/"+lFilesRef[getSelection()]);
    }
    else if(aShuffle) {
      if(getNbDirectories() != getTotalFiles()) {
        int lSelection = rand()%(getTotalFiles()-getNbDirectories());

        setSelection(getNbDirectories()+lSelection);
        playFile(getCurrentDirectory()+"/"+lFilesRef[getSelection()].c_str());
      }
    }
 
	aNowPlaying = false;
	disableCurrentFileIndicator();
	
    return true;
  }
  return false;
}

void Mp3ConsoleMenu::endLoop()
{

}

void Mp3ConsoleMenu::playFile(const std::string& pFileName)
{
  setCurrentFileIndicator(pFileName);

  if(pFileName.find(".mp3") != std::string::npos) {

	std::string command = "echo \"load " + pFileName + "\" >/tmp/mpg123";
	system(command.c_str());
	
  }

}
