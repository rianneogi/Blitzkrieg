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
    begin = system_time_to_msec();
}

void Clock::Stop()
{
	time += system_time_to_msec()-begin;
    begin = system_time_to_msec();
}

void Clock::Reset()
{
	time = 0;
}

unsigned long long Clock::ElapsedMilliseconds()
{
    return time;
}

double Clock::ElapsedSeconds()
{
    return time/(double)1000;

}