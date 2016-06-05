#include "stdafx.h"
#include "Timer.h"


CTimer::CTimer() : lastTime{ Clock::now() }, sampleCount{ 0 }, currentFrameRate{ 0 }, fps{ 0 }, fpsTimeElapsed{ 0.0f }, timeScale{ 0.001f }
{
}


CTimer::~CTimer()
{
}

void CTimer::Tick(float lockFPS)
{
	currentTime = Clock::now();
	auto nanoElapsed{ currentTime - lastTime };
	auto milliElapsed{ std::chrono::duration_cast<std::chrono::milliseconds>(nanoElapsed) };
	float elapsed{ milliElapsed.count() * timeScale };

	if (lockFPS > 0.0f)
	{
		while (elapsed < (1.0f / lockFPS))
		{
			currentTime = Clock::now();
			nanoElapsed = currentTime - lastTime;
			milliElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(nanoElapsed);
			elapsed = milliElapsed.count() * timeScale;
		}
	}

	lastTime = currentTime;

	if (fabsf(elapsed - timeElapsed) < 1.0f)
	{
		memmove_s(frameTime+1, sizeof(float)*(MAX_SAMPLE_COUNT - 1), frameTime, sizeof(float)*(MAX_SAMPLE_COUNT - 1));
		frameTime[0] = elapsed;
		if (sampleCount < MAX_SAMPLE_COUNT) sampleCount++;
	}

	fps++;
	fpsTimeElapsed += elapsed;
	if (fpsTimeElapsed > 1.0f)
	{
		currentFrameRate = fps;
		fps = 0;
		fpsTimeElapsed = 0.0f;
	}

	timeElapsed = 0.0f;
	for (ULONG i = 0; i < sampleCount; ++i)
	{
		timeElapsed += frameTime[i];
	}
	if (sampleCount > 0) timeElapsed /= sampleCount;
}

ULONG CTimer::GetFrameRate(LPTSTR str, int len)
{
	if (str)
	{
		_itow_s(currentFrameRate, str, len, 10);
		wcscat_s(str, len, TEXT(" FPS"));
	}
	
	return currentFrameRate;
}

float CTimer::GetTimeElapsed()
{
	return timeElapsed;
}
