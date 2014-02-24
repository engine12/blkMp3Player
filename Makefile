#############################################################################
# Makefile for building blkMp3Player
#############################################################################

####### Compiler, tools and options

CFLAGS	+=	-pipe -g -fno-exceptions -fno-rtti -Os -fdata-sections -ffunction-sections
CXXFLAGS+=	-pipe -g -fno-exceptions -fno-rtti -Os -fdata-sections -ffunction-sections

LDFLAGS+='-Wl,--gc-sections'	
LIBS	=	$(LDFLAGS) $(SUBLIBS) -lncurses	

####### Files

HEADERS =	ConsoleFileBrowser.h \
		Mp3ConsoleMenu.h \
		fileUtils.h
SOURCES =	ConsoleFileBrowser.cc \
		Mp3ConsoleMenu.cc \
		fileUtils.cc \
		megaChooser.cc
OBJECTS =	obj/ConsoleFileBrowser.o \
		obj/Mp3ConsoleMenu.o \
		obj/fileUtils.o \
		obj/megaChooser.o
	
TARGET	=	blkMp3Player

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	$(CXX) -c $(CFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules


all: blkMp3Player

blkMp3Player: $(OBJECTS)
	$(CXX) $(LFLAGS) $(OBJECTS) -o blkMp3Player $(LIBS)

clean:
	-rm -f $(OBJECTS) $(TARGET)


obj/ConsoleFileBrowser.o: ConsoleFileBrowser.cc \
		ConsoleFileBrowser.h \
		fileUtils.h
	$(CXX) -c $(CFLAGS) $(INCPATH) -o obj/ConsoleFileBrowser.o ConsoleFileBrowser.cc

obj/Mp3ConsoleMenu.o: Mp3ConsoleMenu.cc \
		Mp3ConsoleMenu.h \
		ConsoleFileBrowser.h \
		
	$(CXX) -c $(CFLAGS) $(INCPATH) -o obj/Mp3ConsoleMenu.o Mp3ConsoleMenu.cc

obj/ForkedProcess.o: ForkedProcess.cc \
		fileUtils.h
	$(CXX) -c $(CFLAGS) $(INCPATH) -o obj/ForkedProcess.o ForkedProcess.cc

obj/fileUtils.o: fileUtils.cc \
		fileUtils.h
	$(CXX) -c $(CFLAGS) $(INCPATH) -o obj/fileUtils.o fileUtils.cc

obj/megaChooser.o: megaChooser.cc \
		Mp3ConsoleMenu.h \
		ConsoleFileBrowser.h \

	$(CXX) -c $(CFLAGS) $(INCPATH) -o obj/megaChooser.o megaChooser.cc

