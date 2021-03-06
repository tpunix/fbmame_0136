/*----------- defined in audio/wiping.c -----------*/

extern UINT8 *wiping_soundregs;

DEVICE_GET_INFO( wiping_sound );
#define SOUND_WIPING DEVICE_GET_INFO_NAME(wiping_sound)

WRITE8_HANDLER( wiping_sound_w );


/*----------- defined in video/wiping.c -----------*/

extern UINT8 *wiping_videoram;
extern UINT8 *wiping_colorram;

WRITE8_HANDLER( wiping_flipscreen_w );
PALETTE_INIT( wiping );
VIDEO_UPDATE( wiping );

