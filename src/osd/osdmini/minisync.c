//============================================================
//
//  minisync.c - Minimal core synchronization functions
//
//============================================================
//
//  Copyright Aaron Giles
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or
//  without modification, are permitted provided that the
//  following conditions are met:
//
//    * Redistributions of source code must retain the above
//      copyright notice, this list of conditions and the
//      following disclaimer.
//    * Redistributions in binary form must reproduce the
//      above copyright notice, this list of conditions and
//      the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//    * Neither the name 'MAME' nor the names of its
//      contributors may be used to endorse or promote
//      products derived from this software without specific
//      prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
//  EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGE (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
//  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
//  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//============================================================

#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

#include "osdcore.h"

typedef struct _hidden_mutex_t {
	pthread_mutex_t id;
}hidden_mutex_t;

typedef struct _osd_event {
	pthread_mutex_t     mutex;
	pthread_cond_t      cond;
	volatile INT32      autoreset;
	volatile INT32      signalled;
#ifdef PTR64
	INT8                padding[40];    // Fill a 64-byte cache line
#else
	INT8                padding[48];    // A bit more padding
#endif
}osd_event;

typedef struct _osd_thread {
    pthread_t           thread;
}osd_thread;

typedef struct _osd_work_item
{
	void *result;
}osd_work_item;


//============================================================
//  osd_lock_alloc
//============================================================

osd_lock *osd_lock_alloc(void)
{
	hidden_mutex_t *mutex;
	pthread_mutexattr_t mtxattr;

	mutex = (hidden_mutex_t *)calloc(1, sizeof(hidden_mutex_t));
	if (mutex == NULL)
		return NULL;

	pthread_mutexattr_init(&mtxattr);
	pthread_mutexattr_settype(&mtxattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex->id, &mtxattr);

	return (osd_lock *)mutex;
}


//============================================================
//  osd_lock_acquire
//============================================================

void osd_lock_acquire(osd_lock *lock)
{
	hidden_mutex_t *mutex = (hidden_mutex_t *) lock;
	int r;

	//printf("osd_lock: %p\n", lock);
	r = pthread_mutex_lock(&mutex->id);
	if (r==0)
		return;
	printf("Error on lock: %d: %s\n", r, strerror(r));
}


//============================================================
//  osd_lock_try
//============================================================

int osd_lock_try(osd_lock *lock)
{
	hidden_mutex_t *mutex = (hidden_mutex_t *) lock;
	int r;

	r = pthread_mutex_trylock(&mutex->id);
	if (r==0)
		return 1;
	//if (r!=EBUSY)
	//  osd_printf_error("Error on trylock: %d: %s\n", r, strerror(r));
	return 0;
}


//============================================================
//  osd_lock_release
//============================================================

void osd_lock_release(osd_lock *lock)
{
	hidden_mutex_t *mutex = (hidden_mutex_t *) lock;

	//printf("osd_release: %p\n", lock);
	pthread_mutex_unlock(&mutex->id);
}


//============================================================
//  osd_lock_free
//============================================================

void osd_lock_free(osd_lock *lock)
{
	hidden_mutex_t *mutex = (hidden_mutex_t *) lock;

	//pthread_mutex_unlock(&mutex->id);
	pthread_mutex_destroy(&mutex->id);
	free(mutex);
}


//============================================================
//  osd_event_alloc
//============================================================

osd_event *osd_event_alloc(int manualreset, int initialstate)
{
	osd_event *ev;
	pthread_mutexattr_t mtxattr;

	ev = (osd_event *)calloc(1, sizeof(osd_event));
	if (ev == NULL)
		return NULL;

	pthread_mutexattr_init(&mtxattr);
	pthread_mutex_init(&ev->mutex, &mtxattr);
	pthread_cond_init(&ev->cond, NULL);
	ev->signalled = initialstate;
	ev->autoreset = !manualreset;

	return ev;
}

//============================================================
//  osd_event_free
//============================================================

void osd_event_free(osd_event *event)
{
	pthread_mutex_destroy(&event->mutex);
	pthread_cond_destroy(&event->cond);
	free(event);
}

//============================================================
//  osd_event_set
//============================================================

void osd_event_set(osd_event *event)
{
	pthread_mutex_lock(&event->mutex);
	if (event->signalled == FALSE)
	{
		event->signalled = TRUE;
		if (event->autoreset)
			pthread_cond_signal(&event->cond);
		else
			pthread_cond_broadcast(&event->cond);
	}
	pthread_mutex_unlock(&event->mutex);
}

//============================================================
//  osd_event_reset
//============================================================

void osd_event_reset(osd_event *event)
{
	pthread_mutex_lock(&event->mutex);
	event->signalled = FALSE;
	pthread_mutex_unlock(&event->mutex);
}

//============================================================
//  osd_event_wait
//============================================================

int osd_event_wait(osd_event *event, osd_ticks_t timeout)
{
	if (timeout == OSD_EVENT_WAIT_INFINITE)
		timeout = osd_ticks_per_second() * (osd_ticks_t)10000;

	pthread_mutex_lock(&event->mutex);
	if (!timeout)
	{
		if (!event->signalled)
		{
				pthread_mutex_unlock(&event->mutex);
				return FALSE;
		}
	}
	else
	{
		if (!event->signalled)
		{
			struct timespec   ts;
			struct timeval    tp;
			UINT64 msec = timeout * 1000 / osd_ticks_per_second();
			UINT64 nsec;

			gettimeofday(&tp, NULL);

			ts.tv_sec  = tp.tv_sec;
			nsec = (UINT64) tp.tv_usec * (UINT64) 1000 + (msec * (UINT64) 1000000);
			ts.tv_nsec = nsec % (UINT64) 1000000000;
			ts.tv_sec += nsec / (UINT64) 1000000000;

			do {
				int ret = pthread_cond_timedwait(&event->cond, &event->mutex, &ts);
				if ( ret == ETIMEDOUT )
				{
					if (!event->signalled)
					{
						pthread_mutex_unlock(&event->mutex);
						return FALSE;
					}
					else
						break;
				}
				if (ret == 0)
					break;
				if ( ret != EINTR)
				{
					printf("Error %d while waiting for pthread_cond_timedwait:  %s\n", ret, strerror(ret));
				}

			} while (TRUE);
		}
	}

	if (event->autoreset)
		event->signalled = 0;

	pthread_mutex_unlock(&event->mutex);

	return TRUE;
}

//============================================================
//  osd_thread_create
//============================================================

osd_thread *osd_thread_create(osd_thread_callback callback, void *cbparam)
{
	osd_thread *thread;
	pthread_attr_t  attr;

	thread = (osd_thread *)calloc(1, sizeof(osd_thread));
	if (thread == NULL)
		return NULL;
	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
	if ( pthread_create(&thread->thread, &attr, callback, cbparam) != 0 )
	{
		free(thread);
		return NULL;
	}
	return thread;
}

//============================================================
//  osd_thread_adjust_priority
//============================================================

int osd_thread_adjust_priority(osd_thread *thread, int adjust)
{
	struct sched_param  sched;
	int                 policy;

	if ( pthread_getschedparam( thread->thread, &policy, &sched ) == 0 )
	{
		sched.sched_priority += adjust;
		if ( pthread_setschedparam(thread->thread, policy, &sched ) == 0)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}


//============================================================
//  osd_thread_wait_free
//============================================================

void osd_thread_wait_free(osd_thread *thread)
{
	pthread_join(thread->thread, NULL);
	free(thread);
}



osd_work_queue *osd_work_queue_alloc(int flags)
{
	// this minimal implementation doesn't need to keep any state
	// so we just return a non-NULL pointer
	return (osd_work_queue *)1;
}

int osd_work_queue_items(osd_work_queue *queue)
{
	// we never have pending items
	return 0;
}

int osd_work_queue_wait(osd_work_queue *queue, osd_ticks_t timeout)
{
	// never anything to wait for, so do nothing 
	return TRUE;
}

void osd_work_queue_free(osd_work_queue *queue)
{
	// never allocated anything, so nothing to do
}

osd_work_item *osd_work_item_queue_multiple(osd_work_queue *queue, osd_work_callback callback, INT32 numitems, void *parambase, INT32 paramstep, UINT32 flags)
{
	osd_work_item *item;
	int itemnum;

	// allocate memory to hold the result
	item = (osd_work_item *)malloc(sizeof(*item));
	if (item == NULL)
		return NULL;

	// loop over all requested items
	for (itemnum = 0; itemnum < numitems; itemnum++)
	{
		// execute the call directly
		item->result = (*callback)(parambase, 0);

		// advance the param
		parambase = (UINT8 *)parambase + paramstep;
	}

	// free the item if requested
	if (flags & WORK_ITEM_FLAG_AUTO_RELEASE)
	{
		free(item);
		item = NULL;
	}
	return item;
}


int osd_work_item_wait(osd_work_item *item, osd_ticks_t timeout)
{
	// never anything to wait for, so do nothing
	return TRUE;
}


void *osd_work_item_result(osd_work_item *item)
{
	return item->result;
}


void osd_work_item_release(osd_work_item *item)
{
	free(item);
}


