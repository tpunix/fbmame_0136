//============================================================
//
//  minimisc.c - Minimal core miscellaneous functions
//
//============================================================

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "osdcore.h"



//============================================================
//  osd_alloc_executable
//============================================================

void *osd_alloc_executable(size_t size)
{
	// to use this version of the code, we have to assume that
	// code injected into a malloc'ed region can be safely executed
	return malloc(size);
}


//============================================================
//  osd_free_executable
//============================================================

void osd_free_executable(void *ptr, size_t size)
{
	free(ptr);
}


//============================================================
//  osd_break_into_debugger
//============================================================

void osd_break_into_debugger(const char *message)
{
	// there is no standard way to do this, so ignore it
}


//============================================================
//   osd_ticks
//============================================================

static osd_ticks_t start_sec = 0;

osd_ticks_t osd_ticks(void)
{
		struct timeval    tp;

		gettimeofday(&tp, NULL);
		if (start_sec==0)
			start_sec = tp.tv_sec;
		return (tp.tv_sec - start_sec) * (osd_ticks_t) 1000000 + tp.tv_usec;
}


//============================================================
//  osd_ticks_per_second
//============================================================

osd_ticks_t osd_ticks_per_second(void)
{
	return (osd_ticks_t) 1000000;
}


void osd_sleep(osd_ticks_t duration)
{
	UINT32 msec;

	// convert to milliseconds, rounding down
	msec = (UINT32)(duration * 1000 / osd_ticks_per_second());

	// only sleep if at least 2 full milliseconds
	if (msec >= 2) {
		// take a couple of msecs off the top for good measure
		msec -= 2;
		usleep(msec*1000);
	}

}


