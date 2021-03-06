#include "driver.h"
#include "video/konicdev.h"

int scontra_priority;
static int layer_colorbase[3],sprite_colorbase;

/***************************************************************************

  Callbacks for the K052109

***************************************************************************/

void thunderx_tile_callback(running_machine *machine, int layer,int bank,int *code,int *color,int *flags,int *priority)
{
	*code |= ((*color & 0x1f) << 8) | (bank << 13);
	*color = layer_colorbase[layer] + ((*color & 0xe0) >> 5);
}


/***************************************************************************

  Callbacks for the K051960

***************************************************************************/

void thunderx_sprite_callback(running_machine *machine, int *code,int *color,int *priority_mask,int *shadow)
{
	/* Sprite priority 1 means appear behind background, used only to mask sprites */
	/* in the foreground */
	/* Sprite priority 3 means don't draw (not used) */
	switch (*color & 0x30)
	{
		case 0x00: *priority_mask = 0xf0; break;
		case 0x10: *priority_mask = 0xf0|0xcc|0xaa; break;
		case 0x20: *priority_mask = 0xf0|0xcc; break;
		case 0x30: *priority_mask = 0xffff; break;
	}

	*color = sprite_colorbase + (*color & 0x0f);
}



/***************************************************************************

    Start the video hardware emulation.

***************************************************************************/

VIDEO_START( scontra )
{
	layer_colorbase[0] = 48;
	layer_colorbase[1] = 0;
	layer_colorbase[2] = 16;
	sprite_colorbase = 32;
}


/***************************************************************************

  Display refresh

***************************************************************************/

VIDEO_UPDATE( scontra )
{
	const device_config *k052109 = devtag_get_device(screen->machine, "k052109");
	const device_config *k051960 = devtag_get_device(screen->machine, "k051960");

	k052109_tilemap_update(k052109);

	bitmap_fill(screen->machine->priority_bitmap,cliprect,0);

	/* The background color is always from layer 1 - but it's always black anyway */
//  bitmap_fill(bitmap,cliprect,16 * layer_colorbase[1]);
	if (scontra_priority)
	{
		k052109_tilemap_draw(k052109, bitmap, cliprect, 2, TILEMAP_DRAW_OPAQUE, 1);
		k052109_tilemap_draw(k052109, bitmap, cliprect, 1, 0, 2);
	}
	else
	{
		k052109_tilemap_draw(k052109, bitmap, cliprect, 1, TILEMAP_DRAW_OPAQUE, 1);
		k052109_tilemap_draw(k052109, bitmap, cliprect, 2, 0, 2);
	}
	k052109_tilemap_draw(k052109, bitmap, cliprect, 0, 0, 4);

	k051960_sprites_draw(k051960, bitmap, cliprect, -1, -1);
	return 0;
}
