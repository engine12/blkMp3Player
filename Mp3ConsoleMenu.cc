/*	 Copyright 2012 Mark Majeres  mark@engine12.com		*/

#include "Mp3ConsoleMenu.h"
#include "curses.h"
#include <cstdlib>

#include <linux/input.h>
#include <string.h>

#include<sys/soundcard.h>

#include <fcntl.h>
#include <unistd.h>

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
	bStream = false; 
	bRecieve = false;
	 
	strMetaData = {0};
	nMetaSz=0;
	UpdateMetaData();
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

void Mp3ConsoleMenu::UpdateMetaData()
{
//read /tmp/mpg123_title	
//	const char strMetaData[100];
	static const char tmpfile[] = "/tmp/mpg123_meta";
	FILE *fd = fopen(tmpfile, "r");

	if (fd != NULL) {
		
		fgets(strMetaData, sizeof(strMetaData), fd);
		fclose(fd);
		
	//	StreamTitle='Artist - Track';StreamURL='http://someurl.com';
		
		stream_title = strstr(strMetaData, "StreamTitle='");
		if (stream_title && strlen(stream_title) > 13) {
			char *tmp;
			stream_title+=13;
			tmp = strstr(stream_title, "';");
			if (tmp) tmp[0]  = '\0';
		}
		else
			stream_title = strMetaData;
			
		nMetaSz = strlen(stream_title);
		
		redraw();
	}
}	


void Mp3ConsoleMenu::fileSelected(const std::string& pFile)
{
	if(pFile.empty()) {
		return;
	}
	system("/usr/bin/mpg123.sh");		
	playFile(pFile);
	aNowPlaying = true;
}

void  Mp3ConsoleMenu::onReceive(){
	
	bRecieve = !bRecieve;
	if(bRecieve) {
 			std::string command = "openvt receiveMP3";
			system(command.c_str());
		init_pair(6, COLOR_RED, COLOR_BLACK);
	}
	else{
		std::string command = "stopApp `pidof testMP3Receiver`";
		system(command.c_str());
		init_pair(6, COLOR_WHITE, COLOR_BLACK);
	}
}

void  Mp3ConsoleMenu::onStream(){
	
		bStream = !bStream;
		if(bStream){
			std::string command = "openvt streamMP3";
			system(command.c_str());
			init_pair(7, COLOR_RED, COLOR_BLACK);
		}
		else{
			std::string command = "stopApp `pidof testMP3Streamer`";
			system(command.c_str()); 
			init_pair(7, COLOR_WHITE, COLOR_BLACK);
		}	
}



void  Mp3ConsoleMenu::onVolume(int dev, int bIncrease){
	
	int mixernum=open("/dev/mixer", O_RDWR);
	//first get the volume 
	int vol;
	ioctl(mixernum, MIXER_READ(dev), &vol);
	vol=vol & 0xff;
	bIncrease?vol++:vol--;
	
	int i = vol | vol << 8 ;
	ioctl(mixernum, MIXER_WRITE(dev), &i);
	
	close(mixernum);	
}


#define ZIPIT_VOL_HEAHPHONES 	SOUND_MIXER_ALTPCM

bool Mp3ConsoleMenu::keyEvent(int pKey)
{
/*	Help with finding the keystrokes 
	***Note -- need to change nodelay(stdscr,true); to nodelay(stdscr,false); in the base class constructor too
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
	
	case 260://KEY_LEFT:
		onVolume(ZIPIT_VOL_HEAHPHONES, 0);
		 bRet = false;
		break;
	
	case 261: //KEY_RIGHT:
		onVolume(ZIPIT_VOL_HEAHPHONES, 1);
		 bRet = false;
		break;
	
	case 'o':
		onStream();
		break;

	case 'i':
		onReceive();
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

#define MARQUEE_WIDTH 36
void Mp3ConsoleMenu::headerCallback()
{
	static int i = 0;
	curRow=0;
	//if(nMetaSz > 38)  // the scrolling marquee
	if(--i < 0) i=nMetaSz+MARQUEE_WIDTH;
//	else
	//	i=0;		
	attron(COLOR_PAIR(3));
	attron(WA_DIM);
//	clrtoeol();
	mvprintw(curRow++,0,"--------------- Blacklight MP3 player ---------------");
	attroff(WA_DIM);
	attron(COLOR_PAIR(4));
	wclrtoeol(curscr);
//	int ptr = i<nMetaSz?(nMetaSz-i):0;
	int ptr = i<nMetaSz?(nMetaSz-i):0;
	//if the string is larger then the MARQUEE_WIDTH
//	int nLen = i>nMetaSz?i-nMetaSz+MARQUEE_WIDTH: 0;
	//if the string is less than the width
//	int nLen = i>nMetaSz?i: 0;

	int nLen=0;
	if(i>nMetaSz){
		if(nMetaSz > MARQUEE_WIDTH)
			nLen = i-nMetaSz+MARQUEE_WIDTH;
		else
			nLen = i;
	}
	
//	int nLen = i>MARQUEE_WIDTH?MARQUEE_WIDTH+nMetaSz-i: 0;

	char strPID[MARQUEE_WIDTH+1];
//	snprintf(strPID, sizeof(strPID), "%d", getpid());

	snprintf(strPID, sizeof(strPID),"%*.36s ", nLen , stream_title+ptr );
	
//	mvprintw(curRow++,0," nL= %d: i= %d %.36s ", nLen , i , strPID );
//	mvprintw(curRow++,0," Now Playing: %*.36s ", nLen , strMetaData+ptr );
	mvprintw(curRow++,0," Now Playing: %.36s ", strPID );
}

void Mp3ConsoleMenu::footerCallback()
{
	attron(WA_DIM);
	attron(COLOR_PAIR(3));
	mvprintw(curRow++,0,"-----------------------------------------------------");
	mvprintw(curRow++,0,"  UP/DOWN: ");
	attron(COLOR_PAIR(4));
	printw("Select");
	attron(COLOR_PAIR(3));
	printw("     ENTER: ");
	attron(COLOR_PAIR(4));
	printw("Play");
	attron(COLOR_PAIR(3));
	printw("    Stop: ");
	attron(COLOR_PAIR(4));
	printw("STOP            ");
	attron(COLOR_PAIR(3));
	mvprintw(curRow++,0," r: ");
	attron(COLOR_PAIR(1));
	printw("Repeat");
	attron(COLOR_PAIR(3));
	printw(" s: ");
	attron(COLOR_PAIR(5));
	printw("Shuffle");
	attron(COLOR_PAIR(3));
	printw("  i: ");
	attron(COLOR_PAIR(6));
	printw("mp3_IN");
	attron(COLOR_PAIR(3));
	printw(" o: ");
	attron(COLOR_PAIR(7));
	printw("mp3_OUT");
	attron(COLOR_PAIR(3));	
	printw("  q: ");
	attron(COLOR_PAIR(4));
	printw("Quit ");
	attron(COLOR_PAIR(3));
	mvprintw(curRow++,0,"------------------------------------------------------");

  attroff(WA_DIM);
}

bool Mp3ConsoleMenu::loopIteration()
{
  if(aNowPlaying) {

    const std::vector<std::string>& lFilesRef = getCurrentFiles();

    if(aRepeat) {
      playFile(getCurrentDirectory()+"/"+lFilesRef[getSelection()].c_str());
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
 //if(pFileName.find(".mp3") != std::string::npos) {
	std::string command;
	if(bStream){
		
		//pFileName = pFileName.substr(2,pFileName.size()-2);
		
//		command = "killall catMP3 2>/dev/null; fu=" + pFileName.substr(2,pFileName.size()-2) +"; openvt -s echo \"" + pFileName.substr(2,pFileName.size()-2) +"\"";
//		command = "killall catMP3 2>/dev/null; openvt -s echo " + pFileName.substr(2,pFileName.size()-2) +" >/tmp/shit";
//	command = "killall catMP3 2>/dev/null; fu=" + pFileName.substr(2,pFileName.size()-2) +"; openvt -s echo $fu";
//		command = "killall catMP3 2>/dev/null; openvt catMP3 `" + pFileName +"`";
		command = "killall -9 catMP3 2>/dev/null; openvt catMP3 \"" + pFileName.substr(2,pFileName.size()-2) +"\"; echo \"load /tmp/mp3_in\" >/tmp/mpg123";
	
	}else{
		char strPID[10];
		snprintf(strPID, sizeof(strPID), "%d", getpid());
		setCurrentFileIndicator(pFileName);
		command = "echo \"load " + pFileName + "\" >/tmp/mpg123; echo \"NOTIFY " + std::string(strPID) +"\" >/tmp/mpg123";
	}
	
	system(command.c_str());
	
 // }

}
/*
void Mp3ConsoleMenu::mainLoop()
{
	int lKey = 0;
unsigned int nloops=0;
	int i = 0;
	
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
		flushinp();
        break;
      case KEY_DOWN:
        downKey();
        lRedraw = true;
		flushinp();
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

		if(nMetaSz > 38 && nloops++ > 10){  // the scrolling marquee
			if(i > nMetaSz) i=0;
			{
				setsyx(1, 16);
				printw(" Now Playing: %.38s \n",	strMetaData+i++);
			}
		}
		
    usleep(aLoopSleepTime);

  } while(lKey != 27 && lKey != 'q');

  endLoop();
}
*/