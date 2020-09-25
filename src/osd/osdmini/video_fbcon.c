// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  minimain.c - Main function for mini OSD
//
//============================================================

#include "osdmini.h"


#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <signal.h>
#include <linux/vt.h>
#include <linux/kd.h>


/******************************************************************************/


static int fb_fd;

static struct fb_var_screeninfo g_vinfo;
static int vblank_wait;

extern SIMPLE_QUEUE *fbo_queue;

static osd_thread *vblank_thread;

static int vblank_running;

/******************************************************************************/


static void *video_fbcon_vblank_thread(void *param)
{
	int retv;
	QOBJ *new_obj;
	QOBJ *release_obj = NULL;

	while(vblank_running){
		new_obj = get_ready_qobj(fbo_queue);
		if(new_obj){
			g_vinfo.yoffset = new_obj->data2;
			retv = ioctl(fb_fd, FBIOPAN_DISPLAY, &g_vinfo);
			if(retv<0){
				printk("  FBIOPAN_DISPLAY failed! %d\n", retv);
			}

			if(release_obj){
				qobj_set_idle(release_obj);
			}
			release_obj = new_obj;

			if(vblank_wait){
				osd_sleep(16000);
			}
		}else{
			osd_sleep(3000);
		}
	}

	vblank_running = -1;
	printk("video_fbcon_vblank_thread stop!\n");
	return NULL;
}


/******************************************************************************/


static void check_vblank(void)
{
	INT64 tm_start, tm_end;

	ioctl(fb_fd, FBIOPAN_DISPLAY, &g_vinfo);
	tm_start = osd_ticks();
	ioctl(fb_fd, FBIOPAN_DISPLAY, &g_vinfo);
	tm_end = osd_ticks();
	tm_end -= tm_start;

	vblank_wait = (tm_end<15000)? 1: 0;

	printk("check_vblank_time: %d\n", (int)tm_end);
}

void video_init_fbcon(void)
{
	int i;
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;

	fb_fd = open("/dev/fb0", O_RDWR);
	if(fb_fd<0){
		fatalerror("Open /dev/fb0 failed!\n");
		return -1;
	}

	ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

	fb_xres = vinfo.xres;
	fb_yres = vinfo.yres;
	fb_bpp  = vinfo.bits_per_pixel;
	fb_pitch = finfo.line_length;

	if(vinfo.yres_virtual < fb_yres*3){
		vinfo.yres_virtual = fb_yres*3;
		int retv = ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
		if(retv<0){
			printk("  FBIOPUT_VSCREENINFO failed! %d\n", retv);
		}
	}
	g_vinfo = vinfo;

	fb_base_addr = (UINT8*)mmap(0, finfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);

	printk("/dev/fb0: %dx%d-%d\n", fb_xres, fb_yres, fb_bpp);
	printk("     map: %p  len: %08x\n", fb_base_addr, finfo.smem_len);

	memset(fb_base_addr, 0, finfo.smem_len);

	fbo_queue = simple_queue_create(3);
	for(i=0; i<3; i++){
		qobj_init(&fbo_queue->list[i], (void*)(fb_base_addr + i*fb_yres*fb_pitch), i*fb_yres);
	}

	check_vblank();

	vblank_running = 1;
	vblank_thread = osd_thread_create(video_fbcon_vblank_thread, NULL);
}


void video_exit_fbcon(void)
{
	vblank_running = 0;
	while(vblank_running!=-1){
		osd_sleep(1000);
	}
	simple_queue_free(fbo_queue);
}


/******************************************************************************/


void video_register_fbcon(void)
{
	osd_video_init_backend = video_init_fbcon;
	osd_video_exit_backend = video_exit_fbcon;
}


/******************************************************************************/


