// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  osdmini.h - Core header
//
//============================================================

#ifndef _OSDMINI_H_
#define _OSDMINI_H_


#include "mame.h"
#include "osdcore.h"
#include "osdepend.h"
#include "render.h"


#define printk printf


//============================================================
//  TYPE DEFINITIONS
//============================================================

#define QOBJ_IDLE    0
#define QOBJ_WRITE   1
#define QOBJ_READY   2
#define QOBJ_USING   3

typedef struct _queue_obj
{
	int state;
	int age;
	void *data1;
	int data2;
}QOBJ;

typedef struct {
	int size;
	QOBJ *list;
}SIMPLE_QUEUE;


//============================================================
//  GLOBAL VARIABLES
//============================================================

extern render_target *our_target;
extern running_machine *g_machine;

extern input_device *keyboard_device;
extern UINT8 vt_keystate[];

extern int fb_xres, fb_yres, fb_bpp, fb_pitch;
extern UINT8 *fb_base_addr;

extern float aspect_set;
extern int g_scanline_mode;
extern int g_pause;


//============================================================
//  FUNCTION PROTOTYPES
//============================================================

SIMPLE_QUEUE *simple_queue_create(int size);
void simple_queue_free(SIMPLE_QUEUE *sq);
QOBJ *get_idle_qobj(SIMPLE_QUEUE *sq);
QOBJ *get_ready_qobj(SIMPLE_QUEUE *sq);
void qobj_set_ready(QOBJ *qobj);
void qobj_set_idle(QOBJ *qobj);
void qobj_init(QOBJ *qobj, void *data1, int data2);

void input_keyboard_init(void);

extern void (*osd_input_init)(void);
extern void (*osd_input_update)(void);
extern void (*osd_input_exit)(void);


extern void (*osd_video_init_backend)(void);
extern void (*osd_video_exit_backend)(void);

void osd_video_init(void);
void osd_video_update(int skip_draw);
void osd_video_exit(void);


void input_register_vt(void);
void input_register_remote(void);
void video_register_fbcon(void);
void video_register_remote(void);

void remote_set_state(int state);


int sound_alsa_init(running_machine *machine);
void sound_alsa_exit(void);
void sound_alsa_set_mastervolume(int attenuation);
void sound_alsa_update_stream(const INT16 *buffer, int samples_this_frame);


#endif

