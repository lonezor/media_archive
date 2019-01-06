#include "Video.h"
#include "ShellCmd.h"
#include "Common.h"

#include <string.h>
#include <stdio.h>

Video::Video(File* file) {
	this->file = file;

	memset(&metaData, 0, sizeof(metaData));
}

File::VideoMetaData Video::getMetadata() {
	bool result;
	string cmd;
	string cmdResp;

	memset(&metaData, 0, sizeof(metaData));

	cmd = string("/usr/bin/ffprobe  -v error -of flat=s=_ -select_streams v:0 -show_entries stream=height,width -i ")
			+ "\"" + file->getPath() + "\"";

	cmdResp = ShellCmd::execute(cmd);

	if (cmdResp != "") {
		vector<string> propertyVector;
		vector<string> valueVector;

		propertyVector = Common::splitString(cmdResp, "\n");

		for(vector<string>::iterator it = propertyVector.begin(); it != propertyVector.end(); ++it) {
			string p = *it;

			printf("string '%s'\n", p.c_str());

			if (Common::subStringCount(p, "streams_stream_0_width")) {
				valueVector = Common::splitString(p, "=");
				metaData.width = Common::stringToUint32(valueVector[1]);
				printf("metadata.width=%d\n", metaData.width);

			} else if (Common::subStringCount(p, "streams_stream_0_height")) {
				valueVector = Common::splitString(p, "=");
				metaData.height = Common::stringToUint32(valueVector[1]);
				printf("metadata.height=%d\n", metaData.height);
			}
		}
	}

	cmd = string("/usr/bin/mplayer -identify -frames 0 -vo null -nosound ")
			+ "\"" + file->getPath() + "\" 2>&1 | awk -F= '/LENGTH/{print $2}'";
	cmdResp = ShellCmd::execute(cmd);

	if (cmdResp != "") {
		metaData.duration = Common::stringToFloat(cmdResp);
		printf("metadata.duration=%03f\n", metaData.duration);
	}

	return metaData;
}

Image* Video::getImage(string timestamp) {
	string cmd;
	File* f;
	Image* image;
	bool result;
	string cmdResp;
	/* Extract frame from video and save it in /tmp under hash name *
	 * to avoid file name collisions. 	                            */
	f = new File("/tmp", file->getHashString() + ".png");
	cmd = "/usr/bin/ffmpeg -y -threads 1 -i \"" + file->getPath() + "\" -ss " + timestamp +
		  " -vframes 1 " + f->getPath();

	printf("cmd: %s\n", cmd.c_str());
	cmdResp = ShellCmd::execute(cmd);
	//printf("cmdResp: \n%s", cmdResp.c_str());

	/* Load image */
	image = new Image(f);
	result = image->loadImage();
	if (!result) {
		delete f;
		delete image;
		return NULL;
	}

	/* Remove file from temporary dir */
	cmd = "rm -fv " + f->getPath();
	printf("cmd: %s\n", cmd.c_str());
	cmdResp = ShellCmd::execute(cmd);
	printf("cmdResp: \n%s", cmdResp.c_str());

	delete f;

	if (metaData.width != 0) {
		image->width  = metaData.width;
		image->height = metaData.height;
	}

    return image;
}
