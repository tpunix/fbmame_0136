//============================================================
//
//  minimain.c - Main function for mini OSD
//
//============================================================


#include "osdmini.h"
#include "render.h"
#include "clifront.h"


//============================================================
//  CONSTANTS
//============================================================

typedef struct _kt_table {
	input_item_id   mame_key;
	int             scan_key;
	const char  *   mame_key_name;
	char        *   ui_name;
}kt_table;

#define KTT_ENTRY0(MAME, SDL, UI) { ITEM_ID_ ## MAME, SDL, "ITEM_ID_" #MAME, (char *) UI }
#define KTT_ENTRY1(MAME, SDL) KTT_ENTRY0(MAME, SDL, #MAME)

static kt_table key_trans_table[] =
{
	//         MAME key         scancode
	KTT_ENTRY1(  ESC,           0x01 ),
	KTT_ENTRY1(  1,             0x02 ),
	KTT_ENTRY1(  2,             0x03 ),
	KTT_ENTRY1(  3,             0x04 ),
	KTT_ENTRY1(  4,             0x05 ),
	KTT_ENTRY1(  5,             0x06 ),
	KTT_ENTRY1(  6,             0x07 ),
	KTT_ENTRY1(  7,             0x08 ),
	KTT_ENTRY1(  8,             0x09 ),
	KTT_ENTRY1(  9,             0x0a ),
	KTT_ENTRY1(  0,             0x0b ),
	KTT_ENTRY1(  MINUS,         0x0c ), // -
	KTT_ENTRY1(  EQUALS,        0x0d ), // =
	KTT_ENTRY1(  BACKSPACE,     0x0e ),
	KTT_ENTRY1(  TAB,           0x0f ),
	KTT_ENTRY1(  Q,             0x10 ),
	KTT_ENTRY1(  W,             0x11 ),
	KTT_ENTRY1(  E,             0x12 ),
	KTT_ENTRY1(  R,             0x13 ),
	KTT_ENTRY1(  T,             0x14 ),
	KTT_ENTRY1(  Y,             0x15 ),
	KTT_ENTRY1(  U,             0x16 ),
	KTT_ENTRY1(  I,             0x17 ),
	KTT_ENTRY1(  O,             0x18 ),
	KTT_ENTRY1(  P,             0x19 ),
	KTT_ENTRY1(  OPENBRACE,     0x1a ), // [
	KTT_ENTRY1(  CLOSEBRACE,    0x1b ), // ]
	KTT_ENTRY1(  ENTER,         0x1c ),
	KTT_ENTRY1(  LCONTROL,      0x1d ),
	KTT_ENTRY1(  A,             0x1e ),
	KTT_ENTRY1(  S,             0x1f ),
	KTT_ENTRY1(  D,             0x20 ),
	KTT_ENTRY1(  F,             0x21 ),
	KTT_ENTRY1(  G,             0x22 ),
	KTT_ENTRY1(  H,             0x23 ),
	KTT_ENTRY1(  J,             0x24 ),
	KTT_ENTRY1(  K,             0x25 ),
	KTT_ENTRY1(  L,             0x26 ),
	KTT_ENTRY1(  COLON,         0x27 ), // ;:
	KTT_ENTRY1(  QUOTE,         0x28 ), // '"
	KTT_ENTRY1(  TILDE,         0x29 ), // `~
	KTT_ENTRY1(  LSHIFT,        0x2a ),
	KTT_ENTRY1(  BACKSLASH,     0x2b ), // "\"
	KTT_ENTRY1(  Z,             0x2c ),
	KTT_ENTRY1(  X,             0x2d ),
	KTT_ENTRY1(  C,             0x2e ),
	KTT_ENTRY1(  V,             0x2f ),
	KTT_ENTRY1(  B,             0x30 ),
	KTT_ENTRY1(  N,             0x31 ),
	KTT_ENTRY1(  M,             0x32 ),
	KTT_ENTRY1(  COMMA,         0x33 ), // ,
	KTT_ENTRY1(  STOP,          0x34 ), // .
	KTT_ENTRY1(  SLASH,         0x35 ), // / 
	KTT_ENTRY1(  RSHIFT,        0x36 ),
	KTT_ENTRY1(  ASTERISK,      0x37 ), // KP_*
	KTT_ENTRY1(  LALT,          0x38 ),
	KTT_ENTRY1(  SPACE,         0x39 ),
	KTT_ENTRY1(  CAPSLOCK,      0x3a ),
	KTT_ENTRY1(  F1,            0x3b ),
	KTT_ENTRY1(  F2,            0x3c ),
	KTT_ENTRY1(  F3,            0x3d ),
	KTT_ENTRY1(  F4,            0x3e ),
	KTT_ENTRY1(  F5,            0x3f ),
	KTT_ENTRY1(  F6,            0x40 ),
	KTT_ENTRY1(  F7,            0x41 ),
	KTT_ENTRY1(  F8,            0x42 ),
	KTT_ENTRY1(  F9,            0x43 ),
	KTT_ENTRY1(  F10,           0x44 ),
	KTT_ENTRY1(  NUMLOCK,       0x45 ),
	KTT_ENTRY1(  SCRLOCK,       0x46 ),
	KTT_ENTRY1(  7_PAD,         0x47 ),
	KTT_ENTRY1(  8_PAD,         0x48 ),
	KTT_ENTRY1(  9_PAD,         0x49 ),
	KTT_ENTRY1(  MINUS_PAD,     0x4a ),
	KTT_ENTRY1(  4_PAD,         0x4b ),
	KTT_ENTRY1(  5_PAD,         0x4c ),
	KTT_ENTRY1(  6_PAD,         0x4d ),
	KTT_ENTRY1(  PLUS_PAD,      0x4e ),
	KTT_ENTRY1(  1_PAD,         0x4f ),
	KTT_ENTRY1(  2_PAD,         0x50 ),
	KTT_ENTRY1(  3_PAD,         0x51 ),
	KTT_ENTRY1(  0_PAD,         0x52 ),
	KTT_ENTRY1(  DEL_PAD,       0x53 ),
	// 54-56
	KTT_ENTRY1(  F11,           0x57 ),
	KTT_ENTRY1(  F12,           0x58 ),
	// 59-5f
	KTT_ENTRY1(  ENTER_PAD,     0x60 ),
	KTT_ENTRY1(  RCONTROL,      0x61 ),
	KTT_ENTRY1(  SLASH_PAD,     0x62 ),
	KTT_ENTRY1(  PRTSCR,        0x63 ),
	KTT_ENTRY1(  RALT,          0x64 ),
	// 65-66
	KTT_ENTRY1(  HOME,          0x66 ),
	KTT_ENTRY1(  UP,            0x67 ),
	KTT_ENTRY1(  PGUP,          0x68 ),
	KTT_ENTRY1(  LEFT,          0x69 ),
	KTT_ENTRY1(  RIGHT,         0x6a ),
	KTT_ENTRY1(  END,           0x6b ),
	KTT_ENTRY1(  DOWN,          0x6c ),
	KTT_ENTRY1(  PGDN,          0x6d ),
	KTT_ENTRY1(  INSERT,        0x6e ),
	KTT_ENTRY1(  DEL,           0x6f ),
	// 70-76
	KTT_ENTRY1(  PAUSE,         0x77 ),
	// 78-7c
	KTT_ENTRY1(  LWIN,          0x7d ),
	KTT_ENTRY1(  RWIN,          0x7e ),
	KTT_ENTRY1(  MENU,          0x7f ),

	{ ITEM_ID_INVALID }
};



//============================================================
//  GLOBALS
//============================================================

// a single rendering target
static render_target *our_target;

// a single input device
static input_device *keyboard_device;

// the state of each key
UINT8 vt_keystate[128];

int osdmini_run = 0;


//============================================================
//  FUNCTION PROTOTYPES
//============================================================

static INT32 keyboard_get_state(void *device_internal, void *item_internal);
static void osd_exit(running_machine *machine);


//============================================================
//  main
//============================================================

int main(int argc, char *argv[])
{
	// cli_execute does the heavy lifting; if we have osd-specific options, we
	// would pass them as the third parameter here
	return cli_execute(argc, argv, NULL);
}


//============================================================
//  osd_init
//============================================================

void osd_init(running_machine *machine)
{
	video_init_fbcon();
	sound_alsa_init(machine);

	// initialize the video system by allocating a rendering target
	our_target = render_target_alloc(machine, NULL, 0);
	if (our_target == NULL)
		fatalerror("Error creating render target");

	// nothing yet to do to initialize sound, since we don't have any
	// sound updates are handled by osd_update_audio_stream() below

	// initialize the input system by adding devices
	// let's pretend like we have a keyboard device
	keyboard_device = input_device_add(machine, DEVICE_CLASS_KEYBOARD, "Keyboard", NULL);
	if (keyboard_device == NULL)
		fatalerror("Error creating keyboard device");

	int i = 0;
	while(1){
		if(key_trans_table[i].mame_key == ITEM_ID_INVALID)
			break;

		int sc = key_trans_table[i].scan_key;
		input_device_item_add(keyboard_device,
			key_trans_table[i].ui_name, &vt_keystate[sc], key_trans_table[i].mame_key, keyboard_get_state);
		i += 1;
	}

	osdmini_run = 1;

	add_exit_callback(machine, osd_exit);
}

//============================================================
//  osd_exit
//============================================================


static void osd_exit(running_machine *machine)
{
	printk("osd_exit!\n");

	osdmini_run = 0;

	video_exit_fbcon();
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
	video_update_fbcon(our_target, skip_redraw);
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


//============================================================
//  keyboard_get_state
//============================================================

static INT32 keyboard_get_state(void *device_internal, void *item_internal)
{
	UINT8 *keystate = (UINT8 *)item_internal;
	return *keystate;
}

