/***************************************************************************

 Bishi Bashi Champ Mini Game Senshuken
 (c) 1996 Konami

 Video hardware emulation.

***************************************************************************/

#include "driver.h"
#include "video/konicdev.h"
//#include "includes/konamigx.h"

static int layer_colorbase[4], layerpri[4];

void bishi_tile_callback(running_machine *machine, int layer, int *code, int *color, int *flags)
{
//  *code -= '0';
//  *color = layer_colorbase[layer] | (*color>>2 & 0x0f);
//  K055555GX_decode_vmixcolor(layer, color);
//  if (*color) mame_printf_debug("plane %x col %x [55 %x %x]\n", layer, *color, layer_colorbase[layer], K055555_get_palette_index(layer));

	*color = layer_colorbase[layer] + ((*color & 0xf0));
}

VIDEO_START(bishi)
{
	const device_config *k056832 = devtag_get_device(machine, "k056832");

	assert(video_screen_get_format(machine->primary_screen) == BITMAP_FORMAT_RGB32);

	k056832_set_layer_association(k056832, 0);

	k056832_set_layer_offs(k056832, 0, -2, 0);
	k056832_set_layer_offs(k056832, 1,  2, 0);
	k056832_set_layer_offs(k056832, 2,  4, 0);
	k056832_set_layer_offs(k056832, 3,  6, 0);

	// the 55555 is set to "0x10, 0x11, 0x12, 0x13", but these values are almost correct...
	layer_colorbase[0] = 0x00;
	layer_colorbase[1] = 0x40;	// this one is wrong
	layer_colorbase[2] = 0x80;
	layer_colorbase[3] = 0xc0;
}

/* useful function to sort the three tile layers by priority order */
static void sortlayers(int *layer,int *pri)
{
#define SWAP(a,b) \
	if (pri[a] < pri[b]) \
	{ \
		int t; \
		t = pri[a]; pri[a] = pri[b]; pri[b] = t; \
		t = layer[a]; layer[a] = layer[b]; layer[b] = t; \
	}

	SWAP(0,1)
	SWAP(0,2)
	SWAP(0,3)
	SWAP(1,2)
	SWAP(1,3)
	SWAP(2,3)
}

VIDEO_UPDATE(bishi)
{
	const device_config *k056832 = devtag_get_device(screen->machine, "k056832");
	const device_config *k054338 = devtag_get_device(screen->machine, "k054338");
	const device_config *k055555 = devtag_get_device(screen->machine, "k055555");
	int layers[4], i;/*, old;*/
/*  int bg_colorbase, new_colorbase, plane, dirty; */
	static const int pris[4] = { K55_PRIINP_0, K55_PRIINP_3, K55_PRIINP_6, K55_PRIINP_7 };
	static const int enables[4] = { K55_INP_VRAM_A, K55_INP_VRAM_B, K55_INP_VRAM_C, K55_INP_VRAM_D };

	k054338_update_all_shadows(k054338, 0);
	k054338_fill_backcolor(k054338, bitmap, 0);

	for (i = 0; i < 4; i++)
	{
		layers[i] = i;
		layerpri[i] = k055555_read_register(k055555, pris[i]);
	}

	sortlayers(layers, layerpri);

	bitmap_fill(screen->machine->priority_bitmap,cliprect,0);

	for (i = 0; i < 4; i++)
	{
		if (k055555_read_register(k055555, K55_INPUT_ENABLES) & enables[layers[i]])
		{
			k056832_tilemap_draw(k056832, bitmap, cliprect, layers[i], 0, 1 << i);
		}
	}
	return 0;
}
