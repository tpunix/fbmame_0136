
typedef struct _n8080_state n8080_state;
struct _n8080_state
{
	/* memory pointers */
	UINT8 * videoram;
	UINT8 * colorram;	// for helifire

	/* video-related */
	emu_timer* cannon_timer;
	int spacefev_red_screen;
	int spacefev_red_cannon;
	int sheriff_color_mode;
	int sheriff_color_data;
	int helifire_flash;
	UINT8 helifire_LSFR[63];
	unsigned helifire_mv;
	unsigned helifire_sc; /* IC56 */

	/* sound-related */
	int n8080_hardware;
	emu_timer* sound_timer[3];
	int helifire_dac_phase;
	double helifire_dac_volume;
	double helifire_dac_timing;
	UINT16 prev_sound_pins;
	UINT16 curr_sound_pins;
	int mono_flop[3];
	UINT8 prev_snd_data;

	/* other */
	unsigned shift_data;
	unsigned shift_bits;
	int inte;

	/* devices */
	const device_config *maincpu;
};



/*----------- defined in video/n8080.c -----------*/

WRITE8_HANDLER( n8080_video_control_w );

PALETTE_INIT( n8080 );
PALETTE_INIT( helifire );

VIDEO_START( spacefev );
VIDEO_START( sheriff );
VIDEO_START( helifire );
VIDEO_UPDATE( spacefev );
VIDEO_UPDATE( sheriff );
VIDEO_UPDATE( helifire );
VIDEO_EOF( helifire );

void spacefev_start_red_cannon(running_machine *machine);

/*----------- defined in audio/n8080.c -----------*/

MACHINE_DRIVER_EXTERN( spacefev_sound );
MACHINE_DRIVER_EXTERN( sheriff_sound );
MACHINE_DRIVER_EXTERN( helifire_sound );

MACHINE_START( spacefev_sound );
MACHINE_START( sheriff_sound );
MACHINE_START( helifire_sound );
MACHINE_RESET( spacefev_sound );
MACHINE_RESET( sheriff_sound );
MACHINE_RESET( helifire_sound );

WRITE8_HANDLER( n8080_sound_1_w );
WRITE8_HANDLER( n8080_sound_2_w );
