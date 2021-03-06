/*************************************************************************

    Atari System 1 hardware

*************************************************************************/

#include "machine/atarigen.h"

typedef struct _atarisy1_state atarisy1_state;
struct _atarisy1_state
{
	atarigen_state	atarigen;

	UINT16 *		bankselect;

	UINT8			joystick_type;
	UINT8			trackball_type;

	const device_config *joystick_timer;
	UINT8			joystick_int;
	UINT8			joystick_int_enable;
	UINT8			joystick_value;

	UINT8			tms5220_out_data;
	UINT8			tms5220_in_data;
	UINT8			tms5220_ctl;

	/* playfield parameters */
	UINT16			playfield_lookup[256];
	UINT8			playfield_tile_bank;
	UINT16			playfield_priority_pens;
	const device_config *yscroll_reset_timer;

	/* INT3 tracking */
	int 			next_timer_scanline;
	const device_config *scanline_timer;
	const device_config *int3off_timer;

	/* graphics bank tracking */
	UINT8			bank_gfx[3][8];
	UINT8			bank_color_shift[MAX_GFX_ELEMENTS];
};


/*----------- defined in video/atarisy1.c -----------*/

TIMER_DEVICE_CALLBACK( atarisy1_int3_callback );
TIMER_DEVICE_CALLBACK( atarisy1_int3off_callback );
TIMER_DEVICE_CALLBACK( atarisy1_reset_yscroll_callback );

READ16_HANDLER( atarisy1_int3state_r );

WRITE16_HANDLER( atarisy1_spriteram_w );
WRITE16_HANDLER( atarisy1_bankselect_w );
WRITE16_HANDLER( atarisy1_xscroll_w );
WRITE16_HANDLER( atarisy1_yscroll_w );
WRITE16_HANDLER( atarisy1_priority_w );

VIDEO_START( atarisy1 );
VIDEO_UPDATE( atarisy1 );
