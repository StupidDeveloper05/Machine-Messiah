#include "MicRecord.h"
#include <iostream>

namespace MIC {

	MicRecord::MicRecord(int _channels, int _sample_rate, int _fpb, const char* _outputName)
		: m_channels(_channels)
		, m_sample_rate(_sample_rate)
		, m_fpb(_fpb)
	{
		m_sfinfo.channels = m_channels;
		m_sfinfo.samplerate = m_sample_rate;
		m_sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

		PaError err = Pa_Initialize();
		_process_error("Error initializing PortAudio: ", err);

		m_wavFile = sf_open(_outputName, SFM_WRITE, &m_sfinfo);

		err = Pa_OpenDefaultStream(&m_stream, m_channels, 0, paFloat32, m_sample_rate, m_fpb, recordingCallback, m_wavFile);
		_process_error("Error opening recording stream: ", err);
	}
	
	MicRecord::~MicRecord()
	{
		Pa_Terminate();
	}
	
	void MicRecord::Start()
	{
		PaError err = Pa_StartStream(m_stream);
		_process_error("Error starting recording stream: ", err);
	}
	
	void MicRecord::Pause()
	{
		PaError err = Pa_StopStream(m_stream);
		_process_error("Error stopping recording stream: ", err);

		sf_close(m_wavFile);
	}

	int MicRecord::_process_error(const char* msg, PaError& error)
	{
		if (error != paNoError)
		{
			std::cout << msg << Pa_GetErrorText(error) << std::endl;
			return 1;
		}
	}

	int recordingCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
	{
		sf_writef_float((SNDFILE*)userData, (const float*)inputBuffer, framesPerBuffer);
		return paContinue;
	}
}
