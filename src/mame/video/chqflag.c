/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"
#include "video/konicdev.h"

static int sprite_colorbase,zoom_colorbase[2];

/***************************************************************************

  Callbacks for the K051960

***************************************************************************/

void chqflag_sprite_callback(running_machine *machine, int *code,int *color,int *priority,int *shadow)
{
	*priority = (*color & 0x10) >> 4;
	*color = sprite_colorbase + (*color & 0x0f);
}


/***************************************************************************

  Callbacks for the K051316

***************************************************************************/

void chqflag_zoom_callback_0(running_machine *machine, int *code,int *color,int *flags)
{	*code |= ((*color & 0x03) << 8);
	*color = zoom_colorbase[0] + ((*color & 0x3c) >> 2);
}

void chqflag_zoom_callback_1(running_machine *machine, int *code,int *color,int *flags)
{
	*flags = TILE_FLIPYX((*color & 0xc0) >> 6);
	*code |= ((*color & 0x0f) << 8);
	*color = zoom_colorbase[1] + ((*color & 0x10) >> 4);
}

/***************************************************************************

    Start the video hardware emulation.

***************************************************************************/

VIDEO_START( chqflag )
{
	sprite_colorbase = 0;
	zoom_colorbase[0] = 0x10;
	zoom_colorbase[1] = 0x02;
}

/***************************************************************************

    Display Refresh

***************************************************************************/

VIDEO_UPDATE( chqflag )
{
	const device_config *k051960 = devtag_get_device(screen->machine, "k051960");
	const device_config *k051316_1 = devtag_get_device(screen->machine, "k051316_1");
	const device_config *k051316_2 = devtag_get_device(screen->machine, "k051316_2");

	bitmap_fill(bitmap, cliprect, 0);

	k051316_zoom_draw(k051316_2, bitmap, cliprect, TILEMAP_DRAW_LAYER1, 0);
	k051960_sprites_draw(k051960, bitmap, cliprect, 0, 0);
	k051316_zoom_draw(k051316_2, bitmap, cliprect, TILEMAP_DRAW_LAYER0, 0);
	k051960_sprites_draw(k051960, bitmap, cliprect, 1, 1);
	k051316_zoom_draw(k051316_1, bitmap, cliprect, 0, 0);
	return 0;
}
