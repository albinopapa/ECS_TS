#include "FrameTimer.h"


FrameTimer::FrameTimer()
{
	start = myclock::now();
}

void  FrameTimer::Start()
{
	isStopped = false;
	end = start = myclock::now();	
}
void  FrameTimer::Stop()
{
	isStopped = true;
	end = myclock::now();
}
float FrameTimer::Elapsed()const
{
	if( isStopped )
		return std::chrono::duration<float>( end - start ).count();
	else
		return std::chrono::duration<float>( myclock::now() - start ).count();
}


float FrameTimer::Mark()
{
	return Elapsed();
}
