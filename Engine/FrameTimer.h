#pragma once

#include <chrono>
using myclock = std::chrono::high_resolution_clock;

class FrameTimer
{
public:
	FrameTimer();
	void Start();
	void Stop();
	float Elapsed()const;
	float Mark();
private:	
	myclock::time_point start, end;
	bool isStopped = true;
};

