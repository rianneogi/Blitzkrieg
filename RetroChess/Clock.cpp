#include "Clock.h"


Clock::Clock()
{
	time = 0;
}

Clock::~Clock()
{
}

void Clock::Start()
{
    begin = std::chrono::high_resolution_clock::now();
}

void Clock::Stop()
{
	time += std::chrono::duration_cast<std::chrono::duration<int,std::milli>>(std::chrono::high_resolution_clock::now() - begin).count();
    begin = std::chrono::high_resolution_clock::now();
}

void Clock::Reset()
{
	time = 0;
}

double Clock::ElapsedMilliseconds()
{
    return time;
}

double Clock::ElapsedSeconds()
{
    return time/(double)1000;

}