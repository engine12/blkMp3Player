#############################################################################
# Makefile for building blkMp3Player
# Generated by tmake at 14:06, 2001/01/28
#     Project: _megaChooser
#    Template: app
#############################################################################

####### Compiler, tools and options

#CC	=	gcc
#CXX	=	g++
CFLAGS	+=	-pipe -g -fno-exceptions -fno-rtti -Os -fdata-sections -ffunction-sections
CXXFLAGS+=	-pipe -g -fno-exceptions -fno-rtti -Os -fdata-sections -ffunction-sections
#INCPATH	=	
#LINK	=	g++
LDFLAGS+='-Wl,--gc-sections'	
LIBS	=	$(LDFLAGS) $(SUBLIBS) /home/mark/Downloads/ZipIt/projectgus-openwrt-zipit-4ae4cea/staging_dir/target-arm_v5te_uClibc-0.9.32_eabi/usr/lib/libncurses.so
#MOC	=	$(QTDIR)/bin/moc
UIC	=	

TAR	=	tar -cf
GZIP	=	gzip -9f

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
INTERFACES =	
UICDECLS =	
UICIMPLS =	
SRCMOC	=	
OBJMOC	=	
DIST	=	
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

blkMp3Player: $(UICDECLS) $(OBJECTS) $(OBJMOC) 
	$(CXX) $(LFLAGS) $(OBJECTS) -o blkMp3Player $(LIBS)

moc: $(SRCMOC)

dist:
	$(TAR) _megaChooser.tar _megaChooser.pro $(SOURCES) $(HEADERS) $(INTERFACES) $(DIST)
	$(GZIP) _megaChooser.tar

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core

####### Sub-libraries


###### Combined headers


####### Compile

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

