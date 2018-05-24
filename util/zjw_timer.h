#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
typedef LARGE_INTEGER darco_timeval;
#else
#pragma once

#include <sys/time.h>
#endif

#include <cinttypes>
#include <cstddef>
#include <string>
#include <iostream>

class ZjwTimer {
private:
	darco_timeval tv_start;
	darco_timeval tv_end;

public:
	ZjwTimer() {}
	~ZjwTimer() {}
	void Start();
	void Stop();
	int64_t GetInMs();
	void printTimeInMs(std::string info);
};
