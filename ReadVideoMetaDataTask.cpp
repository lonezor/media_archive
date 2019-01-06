#include "ReadVideoMetaDataTask.h"
#include "Video.h"

ReadVideoMetaDataTask::ReadVideoMetaDataTask(File* file) {
	this->file = file;
}

void ReadVideoMetaDataTask::execute() {
	Video* video;

	if (file->typeIsVideo()) {

		video = new Video(file);

		file->videoMetaData = video->getMetadata();

		delete video;
	}
}
