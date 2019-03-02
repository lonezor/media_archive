#include "File.h"
#include "general.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "GenerateSrcFileHashTask.h"

using namespace std;

/*-------------------------------------------------------------------------------------------------------------------*/

File::File(string dirPath, string name) {
	this->dirPath    = dirPath;
	this->name       = name;
	type       = FILE_TYPE_UNKNOWN;
	size       = 0;
	fsModifiedTs = 0;
	completelyProcessedImages = FALSE;
	completelyProcessedVideos = FALSE;
	completelyProcessedImagesTs = 0;
	completelyProcessedVideosTs = 0;
	imageRotationRequired = FALSE;
	letterboxed = FALSE;
	width = 0;
	height = 0;
	duration = 0;

	memset(this->hash, 0, sizeof(this->hash));

	populateSupportedMetaDataKeyMap();
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
    	} else if (memcmp(ending, ".gif", sizeof(ending)) == 0) {
    		type = FILE_TYPE_GIF;
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
    origAnimation = false;
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

void File::populateSupportedMetaDataKeyMap() {
	supportedMetaDataKeyMap["Image"]                           = "Media Info";
	supportedMetaDataKeyMap["Image.Width"]                     = "Media Info";
	supportedMetaDataKeyMap["Image.Height"]                    = "Media Info";
	supportedMetaDataKeyMap["Animation"]                       = "Media Info";
	supportedMetaDataKeyMap["Animation.Width"]                 = "Media Info";
	supportedMetaDataKeyMap["Animation.Height"]                = "Media Info";
	supportedMetaDataKeyMap["Video"]                           = "Media Info";
	supportedMetaDataKeyMap["Video.Width"]                     = "Media Info";
	supportedMetaDataKeyMap["Video.Height"]                    = "Media Info";
	supportedMetaDataKeyMap["Video.Duration"]                  = "Media Info";
	supportedMetaDataKeyMap["Exif.ApertureValue"]              = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Artist"]                     = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.DateTime"]                   = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ImageDescription"]           = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Make"]                       = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Model"]                      = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.MakerNote"]                  = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.SubSecTimeDigitized"]        = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.SubjectArea"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Orientation"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ResolutionUnit"]             = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Software"]                   = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.XResolution"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.YCbCrPositioning"]           = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.YResolution"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Anti-Blur"]                  = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Brightness"]                 = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.BrightnessValue"]            = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ColorCompensationFilter"]    = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ColorMode"]                  = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ColorReproduction"]          = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ColorSpace"]                 = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ColorTemperature"]           = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ComponentsConfiguration"]    = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.CompressedBitsPerPixel"]     = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Contrast"]                   = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.CustomRendered"]             = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.DateTimeDigitized"]          = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.DateTimeOriginal"]           = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.DigitalZoomRatio"]           = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.DynamicRangeOptimizer"]      = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ExifVersion"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ExposureBiasValue"]          = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ExposureMode"]               = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ExposureProgram"]            = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ExposureTime"]               = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FileFormat"]                 = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FileSrc"]                    = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Flash"]                      = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FlashExposureComp"]          = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FlashLevel"]                 = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FlashPixVersion"]            = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FNumber"]                    = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FocalLength"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FocalLengthIn35mmFilm"]      = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.FullImageSize"]              = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.GainControl"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.HDR"]                        = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.HighISONoiseReduction"]      = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ImageStabilization"]         = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.IntelligentAuto"]            = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ISOSpeedRatings"]            = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.LensCameraation"]            = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.LensModel"]                  = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.LensType"]                   = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.LightSource"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.LongExposureNoiseReduction"] = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.MaxApertureValue"]           = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.MeteringMode"]               = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.MultiFrameNoiseReduction"]   = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Panorama"]                   = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.PixelXDimension"]            = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.PixelYDimension"]            = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.PreviewImageSize"]           = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Quality"]                    = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Saturation"]                 = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.SceneCaptureType"]           = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.SceneType"]                  = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.SensingMethod"]              = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.Sharpness"]                  = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.ShutterSpeedValue"]          = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.SubSecTimeOriginal"]         = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.UserComment"]                = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.WhiteBalance"]               = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.WhiteBalance"]               = "EXIF Camera";
	supportedMetaDataKeyMap["Exif.GPSAltitude"]                = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSAltitudeRef"]             = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSDateStamp"]               = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSDestLatitude"]            = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSDestLatitudeRef"]         = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSDestLongitude"]           = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSDestLongitudeRef"]        = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSImgDirection"]            = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSImgDirectionRef"]         = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSLatitude"]                = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSLatitudeRef"]             = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSLongitude"]               = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSLongitudeRef"]            = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSMeasureMode"]             = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSTimeStamp"]               = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSVersionID"]               = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSDestBearing"]             = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSDestBearingRef"]          = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSSpeed"]           	       = "EXIF GPS";
	supportedMetaDataKeyMap["Exif.GPSSpeedRef"]                = "EXIF GPS";
}

/*-------------------------------------------------------------------------------------------------------------------*/

void File::addMetaData(string key, string value) {
	if (supportedMetaDataKeyMap.count(key) != 0) {
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

	//out += string("SourceFormat " + ); // TODO

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
	bool hashAvailable = false;

	// Has hash been generated?
	uint32_t i;
	for(i=0; i<sizeof(hash); i++) {
		if (hash[i]) {
			hashAvailable = true;
			break;
		}
	}

	if (!hashAvailable) {
		return string("");
	}

	char str[50];
	char hex[3];
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

bool File::typeIsAnimation()
{
	// Unique evaluatation directory is needed. File hashing must be done earlier
	GenerateSrcFileHashTask* generateSrcFileHashTask = new GenerateSrcFileHashTask(this);
	generateSrcFileHashTask->execute();
	delete generateSrcFileHashTask;

	char outDir[512];
	snprintf(outDir, sizeof(outDir), "/tmp/%s", getHashString().c_str());

	char cmd[1024];
	snprintf(cmd, sizeof(cmd), "mkdir -p %s", outDir);
	system(cmd);


	snprintf(cmd, sizeof(cmd), "ffmpeg -i %s %s/output_%%04d.png", getPath().c_str(), outDir);
	system(cmd);

	// If multiple files are found it is an animation
	int nrFiles = 0;
	snprintf(cmd, sizeof(cmd), "ls -l %s | wc -l", outDir);
	FILE* pFile = popen(cmd, "r");
	char line[128];
	fgets(line, sizeof(line), pFile);
	sscanf(line, "%d\n", &nrFiles);
	nrFiles--; // Ignore first status line
	fclose(pFile);

	snprintf(cmd, sizeof(cmd), "rm -rfv %s", outDir);
	printf("cmd %s\n", cmd);
	system(cmd);

	return (nrFiles > 1);
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool File::typeIsImage() {

	// GIF will be transcoded unless it is an animation (which may be designed as loopable and a video doesn't apply either)
	if (type == FILE_TYPE_GIF && typeIsAnimation()) {
		origAnimation = true;
		return false;
	}

	switch (type) {
		case FILE_TYPE_GIF:
		case FILE_TYPE_JPG:
		case FILE_TYPE_PNG:
		case FILE_TYPE_HEIC:
			return true;
		default:
			return false;
	}
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool File::typeIsVideo() {
	return (type == FILE_TYPE_MTS || type == FILE_TYPE_MPG ||
			type == FILE_TYPE_MKV || type == FILE_TYPE_AVI ||
	        type == FILE_TYPE_MP4 || type == FILE_TYPE_WMV ||
	        type == FILE_TYPE_WEBM);
}

/*-------------------------------------------------------------------------------------------------------------------*/

bool File::typeIsHash() {
	return type == FILE_TYPE_HASH;
}

/*-------------------------------------------------------------------------------------------------------------------*/

uint32_t File::getWidth()
{
	return width;
}

/*-------------------------------------------------------------------------------------------------------------------*/

uint32_t File::getHeight()
{
	return height;
}

/*-------------------------------------------------------------------------------------------------------------------*/

uint32_t File::getDuration()
{
	return duration;
}

/*-------------------------------------------------------------------------------------------------------------------*/