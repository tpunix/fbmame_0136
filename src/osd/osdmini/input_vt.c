//============================================================
//
//  input_vt.c - terminal input
//
//============================================================


#include "osdmini.h"


#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/vt.h>
#include <linux/kd.h>


/******************************************************************************/


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


UINT8 vt_keystate[128];

static INT32 keyboard_get_state(void *device_internal, void *item_internal)
{
	UINT8 *keystate = (UINT8 *)item_internal;
	return *keystate;
}

void input_keyboard_init(void)
{
	int i = 0;

	while(1){
		if(key_trans_table[i].mame_key == ITEM_ID_INVALID)
			break;

		int sc = key_trans_table[i].scan_key;
		input_device_item_add(keyboard_device,
			key_trans_table[i].ui_name, &vt_keystate[sc], key_trans_table[i].mame_key, keyboard_get_state);
		i += 1;
	}

	memset(vt_keystate, 0, sizeof(vt_keystate));
}


/******************************************************************************/


static void input_exit_vt(void);
static void sigint_handle(int signum)
{
	printk("\nsigint_handle!\n");
	input_exit_vt();
	exit(1);
}


/******************************************************************************/


static int vtfd, vtkmode, new_vt, current_vt;
static struct termios new_termios, cur_termios;

static void input_init_vt(void)
{
	int fd;
	struct vt_stat vtstate;
	char vpath[16];

	signal(SIGINT, sigint_handle);

	fd = open("/dev/tty0", O_RDWR);
	ioctl(fd, VT_GETSTATE, &vtstate);
	current_vt = vtstate.v_active;
	ioctl(fd, VT_OPENQRY, &new_vt);
	close(fd);


	// fixup current vt
	sprintf(vpath, "/dev/tty%d", current_vt);
	fd = open(vpath, O_RDWR|O_NONBLOCK);
	if(fd>0){
		ioctl(fd, KDGKBMODE, &vtkmode);
		if(vtkmode==K_MEDIUMRAW){
			ioctl(fd, KDSKBMODE, K_XLATE);
			ioctl(fd, KDSETMODE, KD_TEXT);
		}
		close(fd);
	}


	sprintf(vpath, "/dev/tty%d", new_vt);
	vtfd = open(vpath, O_RDWR|O_NONBLOCK);
	if(vtfd>0){
		printk("Switch VT from %d to %d\n", current_vt, new_vt);
		ioctl(vtfd, VT_ACTIVATE, new_vt);
		ioctl(vtfd, VT_WAITACTIVE, new_vt);

		tcgetattr(vtfd, &cur_termios);
		new_termios = cur_termios;
		cfmakeraw(&new_termios);
		tcsetattr(vtfd, TCSAFLUSH, &new_termios);

		ioctl(vtfd, KDSETMODE, KD_GRAPHICS);

		ioctl(vtfd, KDGKBMODE, &vtkmode);
		ioctl(vtfd, KDSKBMODE, K_MEDIUMRAW);
	}else{
		printk("Open %s failed!\n", vpath);
	}

	input_keyboard_init();
}


static void input_exit_vt(void)
{
	if(vtfd>0){
		ioctl(vtfd, KDSKBMODE, vtkmode);

		ioctl(vtfd, KDSETMODE, KD_TEXT);

		tcsetattr(vtfd, TCSAFLUSH, &cur_termios);
		ioctl(vtfd, VT_ACTIVATE, current_vt);
		ioctl(vtfd, VT_WAITACTIVE, current_vt);

		close(vtfd);
		vtfd = -1;
	}
}


static int input_update_vt(void)
{
	int retv;
	UINT8 key;

	while(1){
		retv = read(vtfd, &key, 1);
		if(retv<=0)
			break;

		if(key&0x80){
			vt_keystate[key&0x7f] = 0;
		}else{
			vt_keystate[key&0x7f] = key;
		}
	}

	return key;
}


/******************************************************************************/


void input_register_vt(void)
{
	osd_input_init = input_init_vt;
	osd_input_exit = input_exit_vt;
	osd_input_update = input_update_vt;
}


/******************************************************************************/

