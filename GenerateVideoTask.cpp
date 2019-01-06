#include "GenerateVideoTask.h"
#include "FileSystem.h"
#include "ShellCmd.h"
#include "Video.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

/*-------------------------------------------------------------------------------------------------------------------*/

GenerateVideoTask::GenerateVideoTask(File* file, string targetDirName) {
	this->file          = file;
	this->targetDirName = targetDirName;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void GenerateVideoTask::execute() {
	bool result;
	pid_t pid;
	Video* video;
	File::VideoMetaData metaData;
	bool encode_1k = TRUE;
	bool encode_2k = FALSE;
	bool encode_4k = FALSE;
	string cmd;

	printf("[%s GenerateVideoTask] %s %s\n", __FUNCTION__,
		   file->getPath().c_str(), targetDirName.c_str());

	result = FileSystem::makedir(targetDirName);
	if (!result) {
		printf("[%s] failed to create directory %s\n", __FUNCTION__, targetDirName.c_str());
		return;
	}

	video = new Video(file);

	metaData = video->getMetadata();

	/* Landscape orientation */
	if (file->videoMetaData.width > file->videoMetaData.height) {
		if (file->videoMetaData.width > 3000) {
			encode_4k = TRUE;
		}

		if (file->videoMetaData.width > 1800) {
			encode_2k = TRUE;
		}

		/* Portrait orientation */
	} else {
		if (file->videoMetaData.height > 3000) {
			encode_4k = TRUE;
		}

		if (file->videoMetaData.height > 1800) {
			encode_2k = TRUE;
		}
	}

	/* 1k encoding */ //ffmpeg -i input -c:v libx265 -crf 28 -c:a aac -b:a 128k output.mp4
	cmd = "/usr/bin/ffmpeg -i \"" + file->getPath() + "\" -passlogfile /tmp/" + file->getHashString() +
			" -c:v libx265 -crf 28 -cpu-used 0 -threads 1 -s "
			"1024x576 -aspect 16:9 -y \"" + targetDirName + "/" + file->getHashString()
			+ "_1k.mp4\"";
	printf("cmd '%s'\n", cmd.c_str());
	ShellCmd::execute(cmd);

	cmd = "/bin/rm -fv /tmp/" + file->getHashString() + "-0.log";
	printf("cmd '%s'\n", cmd.c_str());
	ShellCmd::execute(cmd);

	if (encode_2k) {
		cmd = "/usr/bin/ffmpeg -i \"" + file->getPath() + "\" -passlogfile /tmp/" + file->getHashString() +
			" -c:v libx265 -crf 28 -cpu-used 0 -threads 1 -s "
			"1920x1080 -aspect 16:9 -y \"" + targetDirName + "/" + file->getHashString()
			+ "_2k.mp4\"";
		printf("cmd '%s'\n", cmd.c_str());
		ShellCmd::execute(cmd);

		cmd = "/bin/rm -fv /tmp/" + file->getHashString() + "-0.log";
		printf("cmd '%s'\n", cmd.c_str());
		ShellCmd::execute(cmd);
	}

	if (encode_4k) {
		cmd = "/usr/bin/ffmpeg -i \"" + file->getPath() + "\" -passlogfile /tmp/" + file->getHashString() +
			" -c:v libx265 -crf 28 -cpu-used 0 -threads 1 -s "
			"3840x2160 -aspect 16:9 -y \"" + targetDirName + "/" + file->getHashString()
			+ "_4k.mp4\"";
		printf("cmd '%s'\n", cmd.c_str());
		ShellCmd::execute(cmd);

		cmd = "/bin/rm -fv /tmp/" + file->getHashString() + "-0.log";
		printf("cmd '%s'\n", cmd.c_str());
		ShellCmd::execute(cmd);
	}

	// Move original file
	FileSystem::moveFile(file->getPath(), targetDirName + "/" + file->getHashString());

	file->writeInfoFile(targetDirName + "/" + file->getHashString() + ".info");

	file->completelyProcessedVideos = TRUE;
	file->completelyProcessedVideosTs = (uint32_t)time(NULL);

	delete video;

}

/*-------------------------------------------------------------------------------------------------------------------*/
