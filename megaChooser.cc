/*	 Copyright 2012 Mark Majeres  mark@engine12.com		*/

#include "Mp3ConsoleMenu.h"

#include <cstdlib>
#include <signal.h>


void onSignal(int sig)
{
	switch(sig){
	case SIGUSR1:
		thePlayList.onNext();
		break;
	case SIGUSR2:
		thePlayList.UpdateMetaData();
		break;
	default:
		break;
	}
	
}


int main(int argc,char** argv)
{
  srand(time(NULL));
	signal(SIGUSR1, onSignal);
	signal(SIGUSR2, onSignal);

	thePlayList.mainLoop();

	return 0;
}
