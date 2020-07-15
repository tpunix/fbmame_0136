// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  osdmini.h - Core header
//
//============================================================


#include "mame.h"
#include "osdcore.h"
#include "osdepend.h"
#include "render.h"


#define printk printf

extern int osdmini_run;


//============================================================
//  FUNCTION PROTOTYPES
//============================================================


void video_init_fbcon(void);
void video_update_fbcon(render_target *our_target, int skip_draw);
void video_exit_fbcon(void);


int sound_alsa_init(running_machine *machine);
void sound_alsa_exit(void);
void sound_alsa_set_mastervolume(int attenuation);
void sound_alsa_update_stream(const INT16 *buffer, int samples_this_frame);

