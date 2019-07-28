#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define VC_LEANMEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <stdio.h>

#include <mmsystem.h>
#include <mmreg.h>

#include "4klang/4klang.h"

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

int main()
{
	HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
	if (hThread == 0)
	{
		fprintf(stderr, "Failed to create thread\n");
		return 1;
	}

	waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
	waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));

	MMTIME mmTime;
	mmTime.wType = TIME_SAMPLES;
	
	while (!GetAsyncKeyState(VK_ESCAPE)) {
		if (waveOutGetPosition(hWaveOut, &mmTime, sizeof(mmTime)) == MMSYSERR_NOERROR)
		{
			double totalSeconds = (double)mmTime.u.sample / (double)SAMPLE_RATE;
			int minutes = (int)(totalSeconds / 60.0);
			int seconds = (int)totalSeconds % 60;
			int hundredths = (int)(totalSeconds * 100.0) % 100;
			printf("\r %.1i:%.2i.%.2i", minutes, seconds, hundredths);
		}
		
		if (mmTime.u.sample >= MAX_SAMPLES)
			break;

		Sleep(10);
	}

	printf("\n");
}
