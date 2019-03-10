#include "GenerateAudioTask.h"
#include "FileSystem.h"
#include "File.h"
#include "general.h"
#include "ShellCmd.h"
#include "Common.h"

// In the long run the difference between using AAC and FLAC becomes irrelevant. Only images and videos are expected to change with significant increase
// of resolution. Audio stays more or less the same (up to 24bit 192kHz). FLAC is used.

GenerateAudioTask::GenerateAudioTask(File* file, string targetDirName) {
	this->file          = file;
	this->targetDirName = targetDirName;
}

/*-------------------------------------------------------------------------------------------------------------------*/

void GenerateAudioTask::execute() {
	bool result;
	char cmd[2048];

	printf("[%s GenerateAudioTask] %s %s\n", __FUNCTION__,
		   file->getPath().c_str(), targetDirName.c_str());

	result = FileSystem::makedir(targetDirName);
	if (!result) {
		printf("[%s] failed to create directory %s\n", __FUNCTION__, targetDirName.c_str());
		return;
	}

	if (file->type == File::FILE_TYPE_MID) {
		int i;
		char outBasePath[1024];

		snprintf(outBasePath, sizeof(outBasePath), "%s/%s_FluidR3", targetDirName.c_str(), file->getHashString().c_str());

		// Timidity using Fluid R3 soundfont
		snprintf(cmd, sizeof(cmd), "timidity --output-24bit -A120 '%s' -Ow -o '%s_orig.wav'",
			file->getPath().c_str(), outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "sox '%s_orig.wav' '%s.flac' norm -3", // norm-3 to leave headroom for playback-resampling
						outBasePath, outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "rm -f '%s_orig.wav'", outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		// Adlib OPL3 (Yamaha YMF262) player using all available FM patches (from old DOS games)
		for(i=0; i < 66; i++) {
			snprintf(outBasePath, sizeof(outBasePath), "%s/%s_%02d", targetDirName.c_str(), file->getHashString().c_str(), i);

			snprintf(cmd, sizeof(cmd), "adlmidi '%s' -nl -w '%s_orig.wav' %d",
					file->getPath().c_str(), outBasePath, i);
			printf("cmd: '%s'\n", cmd);
			system(cmd);

			snprintf(cmd, sizeof(cmd), "sox '%s_orig.wav' '%s.flac' norm -3", // norm-3 to leave headroom for playback-resampling
						outBasePath, outBasePath);
			printf("cmd: '%s'\n", cmd);
			system(cmd);

			snprintf(cmd, sizeof(cmd), "rm -f '%s_orig.wav'", outBasePath);
			printf("cmd: '%s'\n", cmd);
			system(cmd);
		}

		// Roland MT-32 emulation via munt
		snprintf(outBasePath, sizeof(outBasePath), "%s/%s_Roland_MT-32", targetDirName.c_str(), file->getHashString().c_str());

		// Timidity using Fluid R3 soundfont
		snprintf(cmd, sizeof(cmd), "mt32emu-smf2wav -m /usr/share/sounds/rom/ '%s' -o '%s_orig.wav'", // note: rom path must end with /
			file->getPath().c_str(), outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "sox '%s_orig.wav' '%s.flac' norm -3 rate 48000", // norm-3 to leave headroom for playback-resampling
						outBasePath, outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "rm -f '%s_orig.wav'", outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);
	}
	else if (file->type == File::FILE_TYPE_MOD)
	{
		char outBasePath[1024];

		snprintf(outBasePath, sizeof(outBasePath), "%s/%s", targetDirName.c_str(), file->getHashString().c_str());

		// Timidity MOD playback
		snprintf(cmd, sizeof(cmd), "timidity --output-24bit -A120 '%s' -Ow -o '%s_orig.wav'",
			file->getPath().c_str(), outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "sox '%s_orig.wav' -r 48000 -b 16 '%s.flac' norm -3", // norm-3 to leave headroom for playback-resampling
						outBasePath, outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "rm -f '%s_orig.wav'", outBasePath);
		printf("cmd: '%s'\n", cmd);
		system(cmd);
	}
	else if (file->type == File::FILE_TYPE_MP2 || file->type == File::FILE_TYPE_MP3 ||
		     file->type == File::FILE_TYPE_WAV || file->type == File::FILE_TYPE_FLAC) {

		// Sox doesn't support all encoding formats. Convert to WAV via FFmpeg and then sox to FLAC
		snprintf(cmd, sizeof(cmd), "ffmpeg -i '%s' '%s/%s_orig.wav'", // note: rom path must end with /
			file->getPath().c_str(), targetDirName.c_str(), file->getHashString().c_str());
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "sox '%s/%s_orig.wav' '%s/%s.flac' norm -3 rate 48000", // norm-3 to leave headroom for playback-resampling
					targetDirName.c_str(),
					file->getHashString().c_str(),
					targetDirName.c_str(),
					file->getHashString().c_str());
		printf("cmd: '%s'\n", cmd);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "rm -f '%s/%s_orig.wav'",
					targetDirName.c_str(),
					file->getHashString().c_str());
		printf("cmd: '%s'\n", cmd);
		system(cmd);
	}

	// Move original file
	FileSystem::moveFile(file->getPath(), targetDirName + "/" + file->getHashString());
}

/*-------------------------------------------------------------------------------------------------------------------*/