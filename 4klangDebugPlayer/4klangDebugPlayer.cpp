#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define VC_LEANMEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <stdio.h>

#include <mmsystem.h>
#include <mmreg.h>

#include "4klang/4klang.h"

#define PRERENDER false

static SAMPLE_TYPE lpSoundBuffer[MAX_SAMPLES * 2];
static HWAVEOUT hWaveOut;

static WAVEFORMATEX WaveFMT =
{
#ifdef FLOAT_32BIT	
	WAVE_FORMAT_IEEE_FLOAT,
#else
	WAVE_FORMAT_PCM,
#endif		
	2,                                   // channels
	SAMPLE_RATE,                         // samples per sec
	SAMPLE_RATE * sizeof(SAMPLE_TYPE) * 2, // bytes per sec
	sizeof(SAMPLE_TYPE) * 2,             // block alignment;
	sizeof(SAMPLE_TYPE) * 8,             // bits per sample
	0                                    // extension not needed
};

static WAVEHDR WaveHDR =
{
	(LPSTR)lpSoundBuffer, MAX_SAMPLES * sizeof(SAMPLE_TYPE) * 2, 0, 0, 0, 0, 0, 0
};

static MMTIME MMTime =
{
	TIME_SAMPLES, 0
};


#ifndef NDEBUG
int main()
#else
int WINAPI mainCRTStartup()
#endif
{
#if PRERENDER
	_4klang_render(lpSoundBuffer);
#else
	HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
	if (hThread == 0)
	{
#ifdef _DEBUG
		fprintf(stderr, "Failed to create thread\n");
#endif
		return 1;
	}
#endif

	waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
	waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));

	do {
		waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTime));
#ifdef _DEBUG
		double totalSeconds = (double)MMTime.u.sample / (double)SAMPLE_RATE;
		int minutes = (int)(totalSeconds / 60.0);
		int seconds = (int)totalSeconds % 60;
		int hundredths = (int)(totalSeconds * 100.0) % 100;
		printf("\r %.1i:%.2i.%.2i", minutes, seconds, hundredths);
		Sleep(10);
#endif
	} while (!GetAsyncKeyState(VK_ESCAPE) && MMTime.u.sample < MAX_SAMPLES);

	return 0;
}
