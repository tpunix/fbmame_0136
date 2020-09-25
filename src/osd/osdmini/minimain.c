//============================================================
//
//  minimain.c - Main function for mini OSD
//
//============================================================


#include "osdmini.h"
#include "render.h"
#include "clifront.h"


//============================================================
//  GLOBALS
//============================================================

// a single rendering target
render_target *our_target;
running_machine *g_machine;

// a single input device
input_device *keyboard_device;


void (*osd_input_init)(void) = NULL;
void (*osd_input_update)(void) = NULL;
void (*osd_input_exit)(void) = NULL;


int g_scanline_mode;


//============================================================
//  FUNCTION PROTOTYPES
//============================================================

static void osd_exit(running_machine *machine);


//============================================================
//  main
//============================================================

static const options_entry osdmini_options[] =
{
	{ NULL,                       NULL,       OPTION_HEADER,     "OSDMINI OPTIONS" },
	{ "aspect",                   "0",        0,                 "display aspect" },
	{ "slave",                    "0",        OPTION_BOOLEAN,    "mame work in slave mode" },
    { "scanline",                 "0",        0,                 "display scanline" },
	{ NULL},
};
 


int main(int argc, char *argv[])
{
	// cli_execute does the heavy lifting; if we have osd-specific options, we
	// would pass them as the third parameter here
	return cli_execute(argc, argv, osdmini_options);
}


//============================================================
//  osd_init
//============================================================

void osd_init(running_machine *machine)
{
	g_machine = machine;

	add_exit_callback(machine, osd_exit);


	const char *aspect_value = options_get_string(mame_options(), "aspect");
	if(!strcmp(aspect_value, "auto")){
		aspect_set = 0;
	}else if(!strcmp(aspect_value, "keep")){
		aspect_set = 1111;
	}else if(!strcmp(aspect_value, "4:3")){
		aspect_set = 4.0f/3.0f;
	}else if(!strcmp(aspect_value, "full")){
		aspect_set = 2222;
	}

	g_scanline_mode = options_get_int(mame_options(), "scanline");
	printk("scanline_mode: %d\n", g_scanline_mode);


	if(options_get_bool(mame_options(), "slave")){
		input_register_remote();
		//video_register_remote();
		video_register_fbcon();
	}else{
		input_register_vt();
		video_register_fbcon();
	}


	sound_alsa_init(machine);

	our_target = render_target_alloc(machine, NULL, 0);

	keyboard_device = input_device_add(machine, DEVICE_CLASS_KEYBOARD, "Keyboard", NULL);

	osd_input_init();
	osd_video_init();

}

//============================================================
//  osd_exit
//============================================================


static void osd_exit(running_machine *machine)
{
	printk("osd_exit!\n");

	osd_video_exit();
	osd_input_exit();
	sound_alsa_exit();
}


//============================================================
//  osd_wait_for_debugger
//============================================================

void osd_wait_for_debugger(const device_config *device, int firststop)
{
	// we don't have a debugger, so we just return here
}


//============================================================
//  osd_update
//============================================================

void osd_update(running_machine *machine, int skip_redraw)
{
	osd_video_update(skip_redraw);
	osd_input_update();
}


//============================================================
//  osd_update_audio_stream
//============================================================

void osd_update_audio_stream(running_machine *machine, INT16 *buffer, int samples_this_frame)
{
	sound_alsa_update_stream(buffer, samples_this_frame);
}


//============================================================
//  osd_set_mastervolume
//============================================================

void osd_set_mastervolume(int attenuation)
{
	sound_alsa_set_mastervolume(attenuation);
}


//============================================================
//  osd_customize_input_type_list
//============================================================

void osd_customize_input_type_list(input_type_desc *typelist)
{
}



