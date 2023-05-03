#ifndef MIC_RECORD_H
#define MIC_RECORD_H

#include <portaudio.h>
#include <sndfile.h>

namespace MIC {

	int recordingCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);

	class MicRecord
	{
	public:
		MicRecord(int _channels, int _sample_rate, int _fpb, const char* _outputName="output.wav");
		~MicRecord();

	public:
		void Start();
		void Pause();

	private:
		int _process_error(const char* msg, PaError& error);

	private:
		// mic prop
		int m_channels;
		int m_sample_rate;
		int m_fpb; // frames per buffer

		// wave file
		PaStream* m_stream;
		SNDFILE* m_wavFile;
		SF_INFO m_sfinfo;
	};
}

#endif