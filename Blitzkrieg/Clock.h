#pragma once

#include <chrono>
#include <sys/timeb.h>

class Clock
{
public:
	unsigned long long begin;
    //std::chrono::high_resolution_clock::time_point end;

	unsigned long long time;
	//std::chrono::time_point<std::chrono::high_resolution_clock> t;

	Clock();
	~Clock();
	void Start();
	void Stop();
	void Reset();
	unsigned long long ElapsedMilliseconds();
	double ElapsedSeconds();
};

inline int64_t system_time_to_msec() {
	_timeb t;
	_ftime64_s(&t);
	return t.time * 1000LL + t.millitm;
}

