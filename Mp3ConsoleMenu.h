/*	 Copyright 2012 Mark Majeres  mark@engine12.com		*/

#ifndef Mp3ConsoleMenu_h
#define Mp3ConsoleMenu_h

#include "ConsoleFileBrowser.h"

class Mp3ConsoleMenu : public ConsoleFileBrowser {
public:
  Mp3ConsoleMenu();
  ~Mp3ConsoleMenu();

	void onNext();
	void onPrev();
	void UpdateMetaData(){}
	
	static Mp3ConsoleMenu &getInstance(){return _instance ;}

protected:
  virtual void fileSelected(const std::string& pFile);
  virtual bool keyEvent(int pKey);

  virtual void headerCallback();
  virtual void footerCallback();

  virtual bool loopIteration();
  virtual void endLoop();
private:
  void playFile(const std::string& pFileName);

  bool aPlaylistMode;
  bool aNowPlaying;
  bool aRepeat,aShuffle;

	static Mp3ConsoleMenu _instance;
};

extern Mp3ConsoleMenu& thePlayList;

#endif // Mp3ConsoleMenu_h
