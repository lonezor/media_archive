CC=g++
CXX_FLAGS=-std=c++11 -O0 -g3 -Wall
LIBS=-lpthread -lfreeimage -limagequant -lmhash -lpng

HDR=	general.h \
	Common.h \
	Directory.h \
	Html.h \
	Options.h \
	SharedThreadData.h \
	File.h \
	FileSystem.h \
	Image.h \
	Video.h \
	Media.h \
	ShellCmd.h \
	Task.h \
	GenerateSrcFileHashTask.h \
	GenerateImageTask.h \
	GenerateVideoTask.h \
	ReadImageMetaDataTask.h \
	ReadVideoMetaDataTask.h \
	SyncWithFsTask.h \
	PngReader.h \
	PngWriter.h

OBJ=	main.o \
	Common.o \
	Directory.o \
	Html.o \
	Options.o \
	SharedThreadData.o \
	File.o \
	FileSystem.o \
	Image.o \
	Video.o \
	Media.o \
	ShellCmd.o \
	Task.o \
	GenerateSrcFileHashTask.o \
	GenerateImageTask.o \
	GenerateVideoTask.o \
	ReadImageMetaDataTask.o \
	ReadVideoMetaDataTask.o \
	SyncWithFsTask.o \
	PngReader.o \
	PngWriter.o

%.o: %.cpp $(HDR)
	$(CC) $(CXX_FLAGS) -c $<

mtrans: $(OBJ)
	$(CC) -o $@  *.o $(LIBS)

all: mtrans

install:
	cp -f mtrans /usr/bin

clean:
	rm -f *.o




