#ifndef VIDEO_H_
#define VIDEO_H_

#include "File.h"
#include "Media.h"
#include "Image.h"

#include <string>


using namespace std;

class Video : Media {
public:



	Video(File* file);

	File::VideoMetaData   getMetadata(void);
	Image*     getImage(string timestamp); /* format "HH:MM:SS.msec" */
	bool       encode(string targetDir, string baseFileName, Resolution resolution);

	File* file;
	File::VideoMetaData metaData;
};




#endif /* VIDEO_H_ */
