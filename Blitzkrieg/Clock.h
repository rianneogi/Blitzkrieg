#pragma once

#include <chrono>

class Clock
{
public:
	std::chrono::high_resolution_clock::time_point begin;
    //std::chrono::high_resolution_clock::time_point end;

	unsigned long long time;
	//std::chrono::time_point<std::chrono::high_resolution_clock> t;

	Clock();
	~Clock();
	void Start();
	void Stop();
	void Reset();
	double ElapsedMilliseconds();
	double ElapsedSeconds();

};

