#include "ReadAudioMetaDataTask.h"
#include "FileSystem.h"
#include "PngReader.h"
#include "PngWriter.h"
#include "general.h"
#include "ShellCmd.h"

#include <mhash.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdio.h>
#include <list>
#include <errno.h>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

ReadAudioMetaDataTask::ReadAudioMetaDataTask(File* file) {
	this->file = file;
}

/*-------------------------------------------------------------------------------------------------------------------*/

string ReadAudioMetaDataTask::extractAttributeValue(string line) {
	string value = "";
	size_t pos = line.find(": ");

	if (pos != string::npos) {
		pos += 2;
		size_t left = line.length() - pos;
		value = line.substr(pos,left);
	}

	return value;
}

void ReadAudioMetaDataTask::execute() {
	if (file->typeIsAudio()) {
		printf("[%s] path %s\n", __func__, file->getPath().c_str());

		string out = ShellCmd::execute("ffprobe \"" + file->getPath() + "\"");

		istringstream ss(out);
		string line;
		while (getline(ss, line)) {
			string cmpLine = string(line);
			transform(cmpLine.begin(), cmpLine.end(), cmpLine.begin(), ::tolower);

			string attribute = "";
			string value = "";
			bool extractValue = false;
			
			if (cmpLine.find("artist") != std::string::npos) {
				attribute = "Artist";
				extractValue = true;
			}
			else if (cmpLine.find("album") != std::string::npos) {
				attribute = "Album";
				extractValue = true;
			}
			else if (cmpLine.find("genre") != std::string::npos) {
				attribute = "Genre";
				extractValue = true;
			}
			else if (cmpLine.find("track") != std::string::npos) {
				attribute = "Track";
				extractValue = true;
			}
			else if (cmpLine.find("title") != std::string::npos) {
				attribute = "Title";
				extractValue = true;
			}

			if (extractValue) {
				value = extractAttributeValue(line);
			}

			// In the case of audio metadata, values must be provided
			if (attribute.length() != 0 && value.length() != 0) {
				file->addMetaData(attribute, value); 
			}
		}
	}
}