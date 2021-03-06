

typedef struct _docastle_state docastle_state;
struct _docastle_state
{
	/* memory pointers */
	UINT8 *  videoram;
	UINT8 *  colorram;
	UINT8 *  spriteram;
	size_t   spriteram_size;

	/* video-related */
	tilemap_t  *do_tilemap;

	/* misc */
	int      adpcm_pos, adpcm_idle;
	int      adpcm_data;
	int      adpcm_status;
	UINT8    buffer0[9], buffer1[9];

	/* devices */
	const device_config *slave;
};


/*----------- defined in machine/docastle.c -----------*/

READ8_HANDLER( docastle_shared0_r );
READ8_HANDLER( docastle_shared1_r );
WRITE8_HANDLER( docastle_shared0_w );
WRITE8_HANDLER( docastle_shared1_w );
WRITE8_HANDLER( docastle_nmitrigger_w );

/*----------- defined in video/docastle.c -----------*/

WRITE8_HANDLER( docastle_videoram_w );
WRITE8_HANDLER( docastle_colorram_w );
READ8_HANDLER( docastle_flipscreen_off_r );
READ8_HANDLER( docastle_flipscreen_on_r );
WRITE8_HANDLER( docastle_flipscreen_off_w );
WRITE8_HANDLER( docastle_flipscreen_on_w );

PALETTE_INIT( docastle );
VIDEO_START( docastle );
VIDEO_START( dorunrun );
VIDEO_UPDATE( docastle );

