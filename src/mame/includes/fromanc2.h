
typedef struct _fromanc2_state fromanc2_state;
struct _fromanc2_state
{
	/* memory pointers */
	UINT16   *paletteram[2];
	UINT16   *videoram[2][4];

	/* video-related */
	tilemap_t  *tilemap[2][4];
	int      scrollx[2][4], scrolly[2][4];
	int      gfxbank[2][4];

	/* misc */
	int      portselect;
	UINT8    subcpu_int_flag, subcpu_nmi_flag, sndcpu_nmi_flag;
	UINT16   datalatch1;
	UINT8    datalatch_2h, datalatch_2l;

	/* devices */
	const device_config *audiocpu;
	const device_config *subcpu;
	const device_config *eeprom;
	const device_config *left_screen;
	const device_config *right_screen;
};




/*----------- defined in video/fromanc2.c -----------*/

VIDEO_UPDATE( fromanc2 );
VIDEO_START( fromanc2 );
VIDEO_START( fromancr );
VIDEO_START( fromanc4 );

READ16_HANDLER( fromanc2_paletteram_0_r );
READ16_HANDLER( fromanc2_paletteram_1_r );
WRITE16_HANDLER( fromanc2_paletteram_0_w );
WRITE16_HANDLER( fromanc2_paletteram_1_w );
READ16_HANDLER( fromancr_paletteram_0_r );
READ16_HANDLER( fromancr_paletteram_1_r );
WRITE16_HANDLER( fromancr_paletteram_0_w );
WRITE16_HANDLER( fromancr_paletteram_1_w );
READ16_HANDLER( fromanc4_paletteram_0_r );
READ16_HANDLER( fromanc4_paletteram_1_r );
WRITE16_HANDLER( fromanc4_paletteram_0_w );
WRITE16_HANDLER( fromanc4_paletteram_1_w );
WRITE16_HANDLER( fromanc2_videoram_0_w );
WRITE16_HANDLER( fromanc2_videoram_1_w );
WRITE16_HANDLER( fromanc2_videoram_2_w );
WRITE16_HANDLER( fromanc2_videoram_3_w );
WRITE16_HANDLER( fromancr_videoram_0_w );
WRITE16_HANDLER( fromancr_videoram_1_w );
WRITE16_HANDLER( fromancr_videoram_2_w );
WRITE16_HANDLER( fromanc4_videoram_0_w );
WRITE16_HANDLER( fromanc4_videoram_1_w );
WRITE16_HANDLER( fromanc4_videoram_2_w );
WRITE16_HANDLER( fromanc2_gfxreg_0_w );
WRITE16_HANDLER( fromanc2_gfxreg_1_w );
WRITE16_HANDLER( fromanc2_gfxreg_2_w );
WRITE16_HANDLER( fromanc2_gfxreg_3_w );
WRITE16_HANDLER( fromancr_gfxreg_0_w );
WRITE16_HANDLER( fromancr_gfxreg_1_w );
WRITE16_HANDLER( fromanc2_gfxbank_0_w );
WRITE16_HANDLER( fromanc2_gfxbank_1_w );
void fromancr_gfxbank_w(running_machine *machine, int data);
WRITE16_HANDLER( fromanc4_gfxreg_0_w );
WRITE16_HANDLER( fromanc4_gfxreg_1_w );
WRITE16_HANDLER( fromanc4_gfxreg_2_w );
