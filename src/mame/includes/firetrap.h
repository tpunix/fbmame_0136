/***************************************************************************

    Fire Trap

***************************************************************************/

typedef struct _firetrap_state firetrap_state;
struct _firetrap_state
{
	/* memory pointers */
	UINT8 *       bg1videoram;
	UINT8 *       bg2videoram;
	UINT8 *       fgvideoram;
	UINT8 *       spriteram;
	size_t        spriteram_size;

	/* video-related */
	tilemap_t       *fg_tilemap, *bg1_tilemap, *bg2_tilemap;
	UINT8         scroll1_x[2], scroll1_y[2];
	UINT8         scroll2_x[2], scroll2_y[2];

	/* misc */
	int           irq_enable, nmi_enable;
	int           i8751_return, i8751_current_command;
	int           i8751_init_ptr;
	int           msm5205next;
	int           adpcm_toggle;
	int           int_latch;
	int           coin_command_pending;

	/* devices */
	const device_config *maincpu;
	const device_config *audiocpu;
	const device_config *msm;
};


/*----------- defined in video/firetrap.c -----------*/

WRITE8_HANDLER( firetrap_fgvideoram_w );
WRITE8_HANDLER( firetrap_bg1videoram_w );
WRITE8_HANDLER( firetrap_bg2videoram_w );
WRITE8_HANDLER( firetrap_bg1_scrollx_w );
WRITE8_HANDLER( firetrap_bg1_scrolly_w );
WRITE8_HANDLER( firetrap_bg2_scrollx_w );
WRITE8_HANDLER( firetrap_bg2_scrolly_w );

PALETTE_INIT( firetrap );
VIDEO_START( firetrap );
VIDEO_UPDATE( firetrap );
