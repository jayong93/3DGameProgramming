#include "stdafx.h"
#include "Timer.h"


CTimer::CTimer() : lastTime{ Clock::now() }, sampleCount{ 0 }, currentFrameRate{ 0 }, fps{ 0 }, fpsTimeElapsed{ 0.0f }, timeScale{ 0.001f }, frameCurIdx{ 0 }
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
		frameTime[frameCurIdx++] = elapsed;
		frameCurIdx = frameCurIdx % MAX_SAMPLE_COUNT;
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

	timeElapsed = elapsed;
	avgTimeElapsed = 0.0f;
	for (ULONG i = 0; i < sampleCount; ++i)
	{
		avgTimeElapsed += frameTime[i];
	}
	if (sampleCount > 0) avgTimeElapsed /= sampleCount;
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

float CTimer::GetAvgTimeElapsed()
{
	return avgTimeElapsed;
}
