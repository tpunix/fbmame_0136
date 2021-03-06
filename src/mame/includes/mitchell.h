/*************************************************************************

    Mitchell hardware

*************************************************************************/

typedef struct _mitchell_state mitchell_state;
struct _mitchell_state
{
	/* memory pointers */
	UINT8 *    videoram;
	UINT8 *    colorram;
	size_t     videoram_size;

	/* video-related */
	tilemap_t    *bg_tilemap;
	UINT8      *objram;           /* Sprite RAM */
	int        flipscreen;
	int        video_bank;
	int        paletteram_bank;

	/* sound-related */
	int        sample_buffer;
	int        sample_select;

	/* misc */
	UINT8      port5_kludge;
	int        input_type;
	int        dial[2], dial_selected;
	int        dir[2];
	int        keymatrix;

	/* devices */
	const device_config *audiocpu;
	const device_config *oki;
};


/*----------- defined in video/mitchell.c -----------*/

WRITE8_HANDLER( mgakuen_paletteram_w );
READ8_HANDLER( mgakuen_paletteram_r );
WRITE8_HANDLER( mgakuen_videoram_w );
READ8_HANDLER( mgakuen_videoram_r );
WRITE8_HANDLER( mgakuen_objram_w );
READ8_HANDLER( mgakuen_objram_r );

WRITE8_HANDLER( pang_video_bank_w );
WRITE8_HANDLER( pang_videoram_w );
READ8_HANDLER( pang_videoram_r );
WRITE8_HANDLER( pang_colorram_w );
READ8_HANDLER( pang_colorram_r );
WRITE8_HANDLER( pang_gfxctrl_w );
WRITE8_HANDLER( pang_paletteram_w );
READ8_HANDLER( pang_paletteram_r );

WRITE8_HANDLER( mstworld_gfxctrl_w );
WRITE8_HANDLER( mstworld_video_bank_w );

VIDEO_START( pang );
VIDEO_UPDATE( pang );
