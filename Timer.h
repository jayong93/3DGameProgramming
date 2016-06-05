#pragma once

const ULONG MAX_SAMPLE_COUNT{ 50 };

class CTimer
{
public:
	CTimer();
	virtual ~CTimer();

	void Tick(float lockFPS = 0.0f);
	ULONG GetFrameRate(LPTSTR string = nullptr, int len = 0);
	float GetTimeElapsed();

private:
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
	using Clock = std::chrono::system_clock;

	float timeScale;
	float timeElapsed;
	TimePoint currentTime;
	TimePoint lastTime;

	float frameTime[MAX_SAMPLE_COUNT];
	ULONG sampleCount;
	int frameCurIdx;

	ULONG currentFrameRate;
	ULONG fps;
	float fpsTimeElapsed;
};

