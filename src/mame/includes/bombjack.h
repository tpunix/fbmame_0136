/*************************************************************************

    Bomb Jack

*************************************************************************/

typedef struct _bombjack_state bombjack_state;
struct _bombjack_state
{
	/* memory pointers */
	UINT8 *    videoram;
	UINT8 *    colorram;
	UINT8 *    spriteram;
//  UINT8 *    paletteram;  // currently this uses generic palette handling
	size_t     spriteram_size;

	/* video-related */
	tilemap_t    *fg_tilemap, *bg_tilemap;
	UINT8      background_image;

	/* sound-related */
	UINT8      latch;
};


/*----------- defined in video/bombjack.c -----------*/

WRITE8_HANDLER( bombjack_videoram_w );
WRITE8_HANDLER( bombjack_colorram_w );
WRITE8_HANDLER( bombjack_background_w );
WRITE8_HANDLER( bombjack_flipscreen_w );

VIDEO_START( bombjack );
VIDEO_UPDATE( bombjack );
