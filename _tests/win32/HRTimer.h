#pragma once
#include <exception>
#include <windows.h>

class HRTimer {
public:

	HRTimer(void);
	double  GetFrequency(void);
	void	StartTimer(void) ;
	double  StopTimer(void);

private:
	LARGE_INTEGER start;
	LARGE_INTEGER stop;
	double frequency;
};

HRTimer::HRTimer()
	: start()
	, stop()
{
	frequency = this->GetFrequency();
}

double HRTimer::GetFrequency(void)
{
	LARGE_INTEGER proc_freq;
	if (!::QueryPerformanceFrequency(&proc_freq)) throw std::exception("QueryPerformanceFrequency() failed");
	return (double)proc_freq.QuadPart;
}

void HRTimer::StartTimer(void)
{
	::QueryPerformanceCounter(&start);
}

double HRTimer::StopTimer(void)
{
	::QueryPerformanceCounter(&stop);
	return ((stop.QuadPart - start.QuadPart) / frequency);
} 