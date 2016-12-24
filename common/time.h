#ifdef _TIME_ONCE_H_
#error "please include this file only in main.cpp once!"
#endif

#define _TIME_ONCE_H_

#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <cstdlib>
#include <cstdio>

static timeval start;
static rusage usage;
static float extra;

static void log_memory_end()
{
	getrusage(RUSAGE_SELF, &usage);
	printf("%.3f\n", usage.ru_maxrss / 1024.0 / 1024.0);
}

static void log_time_end()
{
	timeval end;
	gettimeofday(&end, NULL);
	printf("%.3f\n", end.tv_sec - start.tv_sec +
			(end.tv_usec - start.tv_usec) / 1000000.0 + extra);
}

void log_start()
{
	gettimeofday(&start, NULL);
	atexit(log_memory_end);
	atexit(log_time_end);
}

float log_period()
{
	static timeval s, e;
	static bool flag = false;
	static float sum = 0;

	flag = !flag;
	if (flag)
	{
		gettimeofday(&s, NULL);
		return 0;
	} else {
		gettimeofday(&e, NULL);
		sum += e.tv_sec - s.tv_sec +
				(e.tv_usec - s.tv_usec) / 1000000.0;
		return sum;
	}
}

void set_extra_time(float _extra)
{
	extra = _extra;
}

#include <iostream>

class Timer
{
private:
	class Duration
	{
		double period;
	public:
		Duration(double period) : period(period) {}
		friend std::ostream& operator<<(std::ostream &os, const Duration &duration);
	};
	timeval stamp;

public:
	void log()
	{
		gettimeofday(&stamp, nullptr);
	}
	friend Duration operator-(const Timer &, const Timer &);
	friend std::ostream& operator<<(std::ostream &os, const Duration &duration);
};

std::ostream& operator<<(std::ostream &os, const Timer::Duration &duration)
{
	return os << duration.period;
}
Timer::Duration operator-(const Timer &ta, const Timer &tb)
{
	return Timer::Duration(ta.stamp.tv_sec - tb.stamp.tv_sec + (ta.stamp.tv_usec - tb.stamp.tv_usec) / 1000000.0);
}

