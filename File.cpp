#include "File.h"
#include "general.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <iostream>

using namespace std;

/*-------------------------------------------------------------------------------------------------------------------*/

File::File(string dirPath, string name) {
	this->dirPath    = dirPath;
	this->name       = name;
	this->type       = FILE_TYPE_UNKNOWN;
	this->size       = 0;
	this->fsModifiedTs = 0;
	this->completelyProcessedImages = FALSE;
	this->completelyProcessedVideos = FALSE;
	this->completelyProcessedImagesTs = 0;
	this->completelyProcessedVideosTs = 0;
	this->imageRotationRequired = FALSE;
	this->letterboxed = FALSE;

	memset(this->hash, 0, sizeof(this->hash));
	memset(&this->videoMetaData, 0, sizeof(videoMetaData));

	populateSupportedImageMetaDataKeyMap();
}

/*-------------------------------------------------------------------------------------------------------------------*/

void File::determineFileType() {
	this->type = FILE_TYPE_UNKNOWN;

    if (this->name.size() > 5) {
    	int i;
    	int typeLen;
    	int strLen;
    	char ending[5] = {0,0,0,0,0};
    	char* e = (char*)name.c_str();

    	strLen = strlen(e);

    	if (e[strLen-4] == '.') {
    		typeLen = 4;
    	} else if (e[strLen-5] == '.') {
    		typeLen = 5;
    	} else {
    		return;
    	}

    	e += strlen(e)-typeLen;
    	for(i=0; i<typeLen; i++) {
    		ending[i] = tolower(*e);
    		e++;
    	}
    	ending[typeLen] = 0;

    	if (memcmp(ending, ".jpg", sizeof(ending)) == 0) {
    		type = FILE_TYPE_JPG;
    	} else if (memcmp(ending, ".png", sizeof(ending)) == 0) {
    		type = FILE_TYPE_PNG;
    	} else if (memcmp(ending, ".heic", sizeof(ending)) == 0) {
    		type = FILE_TYPE_HEIC;
    	} else if (memcmp(ending, ".mts", sizeof(ending)) == 0) {
    	    type = FILE_TYPE_MTS;
    	} else if (memcmp(ending, ".mpg", sizeof(ending)) == 0) {
    	    type = FILE_TYPE_MPG;
    	} else if (memcmp(ending, ".mkv", sizeof(ending)) == 0) {
    		type = FILE_TYPE_MKV;
    	} else if (memcmp(ending, ".avi", sizeof(ending)) == 0) {
    		type = FILE_TYPE_AVI;
    	} else if (memcmp(ending, ".mp4", sizeof(ending)) == 0) {
    		type = FILE_TYPE_MP4;
    	} else if (memcmp(ending, ".wmv", sizeof(ending)) == 0) {
    		type = FILE_TYPE_WMV;
    	} else if (memcmp(ending, ".webm", sizeof(ending)) == 0) {
    		type = FILE_TYPE_WEBM;
    	} else if (memcmp(ending, ".hash", sizeof(ending)) == 0) {
    		type = FILE_TYPE_HASH;
    	}
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/

void File::analyze() {
	string path = this->getPath();
    struct stat sb;

    determineFileType();

    stat(path.c_str(), &sb);
    this->size         = sb.st_size;
    this->fsModifiedTs = (uint32_t)sb.st_mtim.tv_sec;
}

/*-------------------------------------------------------------------------------------------------------------------*/

string File::getNameWithoutFileType() {
    string name = this->name;
    size_t pos;

    pos = name.find_last_of(".");
    name = name.substr(0,pos);

    return name;
}

string File::getFileEnding() {
	switch (type) {
	case FILE_TYPE_JPG:
		return ".jpg";
		break;
	case FILE_TYPE_PNG:
		return ".png";
		break;
	case FILE_TYPE_UNKNOWN:
	default:
		return "";
	}
}

/*-------------------------------------------------------------------------------------------------------------------*/

void File::print() {
	string path = this->getPath();
	printf("%-100s type=%-5u size=%-10u modified=%-20u\n", path.c_str(), this->type, this->size, this->fsModifiedTs);
}

/*-------------------------------------------------------------------------------------------------------------------*/

string File::getPath() {
	return this->dirPath + "/" + this->name;
}

/*-------------------------------------------------------------------------------------------------------------------*/




void File::populateSupportedImageMetaDataKeyMap() {
	supportedImageMetaDataKeyMap["Exif.ApertureValue"]              = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Artist"]                     = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.DateTime"]                   = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ImageDescription"]           = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Make"]                       = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Model"]                      = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.MakerNote"]                  = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.SubSecTimeDigitized"]        = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.SubjectArea"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Orientation"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ResolutionUnit"]             = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Software"]                   = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.XResolution"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.YCbCrPositioning"]           = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.YResolution"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Anti-Blur"]                  = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Brightness"]                 = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.BrightnessValue"]            = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ColorCompensationFilter"]    = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ColorMode"]                  = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ColorReproduction"]          = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ColorSpace"]                 = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ColorTemperature"]           = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ComponentsConfiguration"]    = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.CompressedBitsPerPixel"]     = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Contrast"]                   = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.CustomRendered"]             = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.DateTimeDigitized"]          = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.DateTimeOriginal"]           = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.DigitalZoomRatio"]           = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.DynamicRangeOptimizer"]      = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ExifVersion"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ExposureBiasValue"]          = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ExposureMode"]               = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ExposureProgram"]            = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ExposureTime"]               = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FileFormat"]                 = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FileSrc"]                    = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Flash"]                      = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FlashExposureComp"]          = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FlashLevel"]                 = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FlashPixVersion"]            = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FNumber"]                    = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FocalLength"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FocalLengthIn35mmFilm"]      = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.FullImageSize"]              = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.GainControl"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.HDR"]                        = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.HighISONoiseReduction"]      = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ImageStabilization"]         = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.IntelligentAuto"]            = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ISOSpeedRatings"]            = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.LensCameraation"]            = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.LensModel"]                  = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.LensType"]                   = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.LightSource"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.LongExposureNoiseReduction"] = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.MaxApertureValue"]           = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.MeteringMode"]               = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.MultiFrameNoiseReduction"]   = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Panorama"]                   = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.PixelXDimension"]            = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.PixelYDimension"]            = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.PreviewImageSize"]           = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Quality"]                    = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Saturation"]                 = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.SceneCaptureType"]           = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.SceneType"]                  = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.SensingMethod"]              = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.Sharpness"]                  = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.ShutterSpeedValue"]          = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.SubSecTimeOriginal"]         = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.UserComment"]                = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.WhiteBalance"]               = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.WhiteBalance"]               = "EXIF Camera";
	supportedImageMetaDataKeyMap["Exif.GPSAltitude"]                = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSAltitudeRef"]             = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSDateStamp"]               = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSDestLatitude"]            = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSDestLatitudeRef"]         = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSDestLongitude"]           = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSDestLongitudeRef"]        = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSImgDirection"]            = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSImgDirectionRef"]         = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSLatitude"]                = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSLatitudeRef"]             = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSLongitude"]               = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSLongitudeRef"]            = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSMeasureMode"]             = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSTimeStamp"]               = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSVersionID"]               = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSDestBearing"]             = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSDestBearingRef"]          = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSSpeed"]           	    = "EXIF GPS";
	supportedImageMetaDataKeyMap["Exif.GPSSpeedRef"]                = "EXIF GPS";
}

/*-------------------------------------------------------------------------------------------------------------------*/

void File::addMetaData(string key, string value) {
	if (supportedImageMetaDataKeyMap.count(key) != 0) {
		printf("adding MetaData key '%s' with value '%s'\n", key.c_str(), value.c_str());
		imageMetaDataMap[key] = value;
	} else {
		printf("skipping MetaData key '%s' with value '%s'\n", key.c_str(), value.c_str());
	}
}

/*-------------------------------------------------------------------------------------------------------------------*/

string File::getMetaData(string key) {
	if (imageMetaDataMap.count(key) != 0) {
		return imageMetaDataMap[key];
	} else {
		return "";
	}
}

void File::writeInfoFile(string path) {
	map<string, string>::iterator it;
	string out = "";

	if (typeIsImage()) {
		out += string("Image\n");

		// Image.Width
		// Image.height
	}

	if (typeIsVideo()) {
		out += string("Video\n");

		if (videoMetaData.width > 0) {
			out += string("Video.width ") + to_string(videoMetaData.width) + "\n";
		}

		if (videoMetaData.height > 0) {
			out += string("Video.height ") + to_string(videoMetaData.height) + "\n";
		}

		if (videoMetaData.duration > 0) {
			out += string("Video.duration ") + to_string(videoMetaData.duration) + "\n";
		}
	}

	if (letterboxed) {
		out += string("Orientation Portrait\n");
	} else {
		out += string("Orientation Landscape\n");
	}

	out += string("Size ") + to_string(size) + string("\n");

	// Sometimes file system timestamps are not preserved
	// and may use a newer timestamp
	// If available, use Exif timestamp.
	time_t ts = (time_t)fsModifiedTs;
	for (it = imageMetaDataMap.begin(); it != imageMetaDataMap.end(); it++)
	{
		if (it->first == "Exif.DateTimeOriginal")
		{
			char dateTime[64];
			snprintf(dateTime, sizeof(dateTime), "%s", it->second.c_str());
			printf("dateTime '%s'\n", dateTime);
			
			// Parse dateTime, format: '2018:05:20 16:26:02'
			dateTime[4] = 0;
			dateTime[7] = 0;
			dateTime[10] = 0;
			dateTime[13] = 0;
			dateTime[16] = 0;

			int year = atoi(&dateTime[0]);
			int month = atoi(&dateTime[5]);
			int day = atoi(&dateTime[8]);
			int hour = atoi(&dateTime[11]);
			int minute = atoi(&dateTime[14]);
			int second = atoi(&dateTime[17]);
			printf("year: %d\n"
				   "month: %d\n"
				   "day: %d\n"
				   "hour: %d\n"
				   "minute: %d\n"
				   "second: %d\n",
				   year,
				   month,
				   day,
				   hour,
				   minute,
				   second);

			 struct tm t;
    		 t.tm_year = year - 1900;
    		 t.tm_mon = month - 1;
    		 t.tm_mday = day;        
    		 t.tm_hour = hour;
		     t.tm_min = minute;
   		     t.tm_sec = second;
   		     t.tm_isdst = -1;        // DST: 1 = yes, 0 = no, -1 = unknown
    		 ts = mktime(&t);
		}
	}

	out += string("Time.Epoch ") + to_string(fsModifiedTs) + string("\n");	

	struct tm * tInfo;
	char tStr[200];
	tInfo = localtime(&ts);

	strftime(tStr, sizeof(tStr), "%Y-%m-%d", tInfo);
	out += string("Time.Date ") + string(tStr) + string("\n");

	strftime(tStr, sizeof(tStr), "%Y", tInfo);
	out += string("Time.Year ") + string(tStr) + string("\n");

	strftime(tStr, sizeof(tStr), "%B", tInfo);
	out += string("Time.Month ") + string(tStr) + string("\n");

	strftime(tStr, sizeof(tStr), "%d", tInfo);
	out += string("Time.Day ") + string(tStr) + string("\n");

	strftime(tStr, sizeof(tStr), "%A", tInfo);
	out += string("Time.Weekday ") + string(tStr) + string("\n");

	strftime(tStr, sizeof(tStr), "%H", tInfo);
	out += string("Time.Hour ") + string(tStr) + string("\n");

	strftime(tStr, sizeof(tStr), "%M", tInfo);
	out += string("Time.Minute ") + string(tStr) + string("\n");

	strftime(tStr, sizeof(tStr), "%S", tInfo);
	out += string("Time.Second ") + string(tStr) + string("\n");

	for (it = imageMetaDataMap.begin(); it != imageMetaDataMap.end(); it++)
	{
    	out +=  it->first + string(" ") + it->second + string("\n");
	}

	FILE* fileStream = fopen(path.c_str(), "w");
	if (!fileStream) {
		printf("error, could not open %s\n", path.c_str());
		return;
	}

	fprintf(fileStream, "%s", out.c_str());
	fclose(fileStream);
}

/*-------------------------------------------------------------------------------------------------------------------*/

string File::getHashString() {
	char str[50];
	char hex[3];
	uint32_t i;
	uint32_t left = sizeof(str) - 1;

	str[0] = 0;

	for(i=0; i<sizeof(hash); i++) {
		snprintf(hex, sizeof(hex), "%02x", hash[i]);
		strncat(str, hex, left);
		left -= 2;
	}

	return string(str);
}

/*-------------------------------------------------------------------------------------------------------------------*/
bool File::typeIsImage() {
	return (type == FILE_TYPE_JPG || type == FILE_TYPE_PNG || 
		    type == FILE_TYPE_HEIC);
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool File::typeIsVideo() {
	return (type == FILE_TYPE_MTS || type == FILE_TYPE_MPG ||
			type == FILE_TYPE_MKV || type == FILE_TYPE_AVI ||
	        type == FILE_TYPE_MP4 || type == FILE_TYPE_WMV ||
	        type == FILE_TYPE_WEBM);
}

bool File::typeIsHash() {
	return type == FILE_TYPE_HASH;
}

/*-------------------------------------------------------------------------------------------------------------------*/
