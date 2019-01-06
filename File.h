#ifndef FILE_H_
#define FILE_H_

#include <string>
#include <stdint.h>
#include <map>

#define FILE_HASH_SIZE 20

using namespace std;

class File {

public:

	enum FileType {
		FILE_TYPE_UNKNOWN = 0,
		FILE_TYPE_JPG     = 1,
		FILE_TYPE_PNG     = 2,
		FILE_TYPE_HEIC    = 3,
		FILE_TYPE_MTS     = 4,
		FILE_TYPE_MPG     = 5,
		FILE_TYPE_MKV     = 6,
		FILE_TYPE_AVI     = 7,
		FILE_TYPE_MP4     = 8,
		FILE_TYPE_WMV     = 9,
		FILE_TYPE_WEBM    = 10,
		FILE_TYPE_HASH    = 11,
	};

	struct VideoMetaData {
		uint32_t width;
		uint32_t height;
		float    duration;
	};

	File(string dirPath, string name);

	string                  dirPath;
	string                  name;
	FileType                type;
	uint32_t                size;
	uint32_t                fsModifiedTs;      /* Timestamp of last file change according to file system record  */
	uint8_t                 hash[FILE_HASH_SIZE];
	bool                    completelyProcessedImages;
	bool                    completelyProcessedVideos;
	uint32_t                completelyProcessedImagesTs;
	uint32_t                completelyProcessedVideosTs;
	std::map<string,string> imageMetaDataMap;
	std::map<string,string> supportedImageMetaDataKeyMap;
	VideoMetaData           videoMetaData;
	bool                    imageRotationRequired;
	bool                    letterboxed;

	void   analyze();
	string getPath();
	string getNameWithoutFileType();
	string getFileEnding();
	void   print();
	void   populateSupportedImageMetaDataKeyMap(void);
	void   addMetaData(string key, string value);
	string getMetaData(string key);
	string getHashString(void);

	void determineFileType();
	void calculateCheckSum();
	void writeChecksumFile();
	void writeInfoFile(string path);

	bool typeIsImage();
	bool typeIsVideo();
	bool typeIsHash();
};


#endif /* FILE_H_ */
