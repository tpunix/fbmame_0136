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

#define USE_THREAD_RENDER

//#define USE_CURRENT_VT

static int vtfd, vtkmode, new_vt, current_vt;
static struct termios new_termios, cur_termios;
extern UINT8 vt_keystate[128];

static void vt_init(void)
{
	int fd;
	struct vt_stat vtstate;
	char vpath[16];

	memset(vt_keystate, 0, sizeof(vt_keystate));

	fd = open("/dev/tty0", O_RDWR);
	ioctl(fd, VT_GETSTATE, &vtstate);
	current_vt = vtstate.v_active;
#ifdef USE_CURRENT_VT
	new_vt = current_vt;
#else
	ioctl(fd, VT_OPENQRY, &new_vt);
#endif
	close(fd);


	// fixup current vt
	sprintf(vpath, "/dev/tty%d", current_vt);
	fd = open(vpath, O_RDWR|O_NONBLOCK);
	if(fd>0){
		ioctl(fd, KDGKBMODE, &vtkmode);
		if(vtkmode==K_MEDIUMRAW){
			ioctl(fd, KDSKBMODE, K_XLATE);
			ioctl(fd, KDSETMODE, KD_TEXT);
		}
		close(fd);
	}


	sprintf(vpath, "/dev/tty%d", new_vt);
	vtfd = open(vpath, O_RDWR|O_NONBLOCK);
	if(vtfd>0){
#ifdef USE_CURRENT_VT
		printk("Use current VT %d\n", new_vt);
#else
		printk("Switch VT from %d to %d\n", current_vt, new_vt);
		ioctl(vtfd, VT_ACTIVATE, new_vt);
		ioctl(vtfd, VT_WAITACTIVE, new_vt);
#endif

		tcgetattr(vtfd, &cur_termios);
		new_termios = cur_termios;
		cfmakeraw(&new_termios);
		tcsetattr(vtfd, TCSAFLUSH, &new_termios);

		ioctl(vtfd, KDSETMODE, KD_GRAPHICS);

		ioctl(vtfd, KDGKBMODE, &vtkmode);
		ioctl(vtfd, KDSKBMODE, K_MEDIUMRAW);
	}else{
		printk("Open %s failed!\n", vpath);
	}

}

static void vt_exit(void)
{
	if(vtfd>0){
		ioctl(vtfd, KDSKBMODE, vtkmode);

		ioctl(vtfd, KDSETMODE, KD_TEXT);

		tcsetattr(vtfd, TCSAFLUSH, &cur_termios);
#ifndef USE_CURRENT_VT
		ioctl(vtfd, VT_ACTIVATE, current_vt);
		ioctl(vtfd, VT_WAITACTIVE, current_vt);
#endif

		close(vtfd);
		vtfd = -1;
	}
}

static int vt_update_key(void)
{
	int retv;
	UINT8 key;

	while(1){
		retv = read(vtfd, &key, 1);
		if(retv<=0)
			break;

		if(key&0x80){
			vt_keystate[key&0x7f] = 0;
		}else{
			vt_keystate[key&0x7f] = key;
		}
	}

	return key;
}

static void sigint_handle(int signum)
{
	printk("\nsigint_handle!\n");
	vt_exit();
	exit(1);
}



//============================================================
//  tripple buffer
//============================================================

#define QOBJ_IDLE    0
#define QOBJ_WRITE   1
#define QOBJ_READY   2
#define QOBJ_USING   3

typedef struct _queue_obj
{
	int state;
	int age;
	void *data1;
	int data2;
}QOBJ;

typedef struct {
	int size;
	QOBJ *list;
}SIMPLE_QUEUE;

static int qobj_ages = 1;

SIMPLE_QUEUE *simple_queue_create(int size)
{
	SIMPLE_QUEUE *sq;

	sq = (SIMPLE_QUEUE*)malloc(sizeof(SIMPLE_QUEUE));

	sq->size = size;
	sq->list = (QOBJ*)malloc(size*sizeof(QOBJ));

	memset(sq->list, 0, size*sizeof(QOBJ));
	return sq;
}

void simple_queue_free(SIMPLE_QUEUE *sq)
{
	free(sq->list);
	free(sq);
}


static QOBJ *get_idle_qobj(SIMPLE_QUEUE *sq)
{
	int i;

	for(i=0; i<sq->size; i++){
		if(sq->list[i].state==QOBJ_IDLE){
			sq->list[i].state = QOBJ_WRITE;
			return &(sq->list[i]);
		}
	}

	//printk("No IDLE QOBJ!\n");
	return NULL;
}

static QOBJ *get_ready_qobj(SIMPLE_QUEUE *sq)
{
	QOBJ *qobj;
	int i, r, age;

	r = -1;
	age = 0x7fffffff;

	for(i=0; i<sq->size; i++){
		qobj = &sq->list[i];
		if(sq->list[i].state==QOBJ_READY){
			if(sq->list[i].age<age){
				r = i;
				age = sq->list[i].age;
			}
		}
	}

	if(r==-1){
		//printk("No READY QOBJ!\n");
		return NULL;
	}

	sq->list[r].state = QOBJ_USING;
	return &sq->list[r];
}


static void qobj_set_ready(QOBJ *qobj)
{
	qobj->age = qobj_ages++;
	qobj->state = QOBJ_READY;
}

static void qobj_set_idle(QOBJ *qobj)
{
	qobj->age = 0;
	qobj->state = QOBJ_IDLE;
}

static void qobj_init(QOBJ *qobj, void *data1, int data2)
{
	qobj->state = QOBJ_IDLE;
	qobj->age = 0;
	qobj->data1 = data1;
	qobj->data2 = data2;
}


/******************************************************************************/


static int fb_fd, fb_xres, fb_yres, fb_bpp, fb_pitch;
static UINT8 *fb_base_addr;

static struct fb_var_screeninfo g_vinfo;
static int vblank_wait;


static SIMPLE_QUEUE *fbo_queue;
static SIMPLE_QUEUE *render_queue;

#ifdef USE_THREAD_RENDER
static osd_event *render_event;
static osd_thread *render_thread;
static osd_thread *vblank_thread;
static void *video_fbcon_render_thread(void *param);
static void *video_fbcon_vblank_thread(void *param);
#endif


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

static int fb_init(void)
{
	int i, retv;
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
		retv = ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
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


	render_queue = simple_queue_create(4);
#ifdef USE_THREAD_RENDER
	render_event = osd_event_alloc(0, 0);
	render_thread = osd_thread_create(video_fbcon_render_thread, NULL);
	vblank_thread = osd_thread_create(video_fbcon_vblank_thread, NULL);
#endif

	return 0;
}



//============================================================
//  show fps
//============================================================

static INT64 vtm_old=0, vtm_new, vtm_sec=0;
static int fps=0;

void video_show_fps(void)
{
	fps += 1;
	vtm_new = osd_ticks();
	if(vtm_new - vtm_sec >= 1000000){
		printk("  fps: %d\n", fps);
		fps = 0;
		vtm_sec = vtm_new;
	}
	//printk("video: %12d %06d\n", (int)vtm_new, (int)(vtm_new-vtm_old));
	vtm_old = vtm_new;
}


//============================================================
//  osd_update
//============================================================


static int game_width=0, game_height=0;
static int fb_draw_w=0, fb_draw_h=0;
static int fb_draw_offset = 0;

static void rgb888_draw_primitives(const render_primitive *primlist, void *dstdata, UINT32 width, UINT32 height, UINT32 pitch);

static void video_swap_buffer(QOBJ *display)
{
	int retv;

	if(display){
		g_vinfo.yoffset = display->data2;
	}

	// PAN_DISPLAY已经有等待VSYNC的动作.
	retv = ioctl(fb_fd, FBIOPAN_DISPLAY, &g_vinfo);
	if(retv<0){
		printk("  FBIOPAN_DISPLAY failed! %d\n", retv);
	}

}


static void do_render(void)
{
	QOBJ *draw_obj;
	QOBJ *render_obj;
	UINT8 *fb_draw_ptr;
	render_primitive_list *primlist;

	video_show_fps();

	render_obj = get_ready_qobj(render_queue);
	primlist = (render_primitive_list*)render_obj->data1;
	qobj_set_idle(render_obj);

	//while(1){
		draw_obj = get_idle_qobj(fbo_queue);
	//	if(draw_obj)
	//		break;
	//	osd_sleep(2000);
	//}
	//printk("\ndraw: %p\n", draw_obj);
	
	if(draw_obj){
		fb_draw_ptr = (UINT8*)(draw_obj->data1) + fb_draw_offset;
		// do the drawing here
		rgb888_draw_primitives(primlist->head, fb_draw_ptr, fb_draw_w, fb_draw_h, fb_pitch/4);
		qobj_set_ready(draw_obj);
	}

	osd_lock_release(primlist->lock);
	render_target_free_primitives(primlist);

}


void video_update_fbcon(render_target *our_target, int skip_draw)
{
	int min_draw_width, min_draw_height;
	render_primitive_list *primlist;
	QOBJ *render_obj;

	if(skip_draw){
		vt_update_key();
		return;
	}

	// get the minimum width/height for the current layout
	render_target_get_minimum_size(our_target, &min_draw_width, &min_draw_height);

	if(game_width!=min_draw_width || game_height!=min_draw_height){
		printk("Change res to %dx%d\n", min_draw_width, min_draw_height);
		game_width = min_draw_width;
		game_height = min_draw_height;

		fb_draw_w = (min_draw_width * fb_yres) / min_draw_height;
		if(fb_draw_w<fb_xres){
			fb_draw_h = fb_yres;
			fb_draw_offset = ((fb_xres-fb_draw_w)/2)*(fb_bpp/8);
		}else{
			fb_draw_h = (min_draw_height * fb_xres) / min_draw_width;
			fb_draw_w = fb_xres;
			fb_draw_offset = ((fb_yres-fb_draw_h)/2)*fb_pitch;
		}
		printk("Scale: %dx%d\n", fb_draw_w, fb_draw_h);

	}

	// make that the size of our target
	render_target_set_bounds(our_target, fb_draw_w, fb_draw_h, 0);

	// get the list of primitives for the target at the current size
	primlist = (render_primitive_list*)render_target_get_primitives(our_target);
	// lock them, and then render them
	osd_lock_acquire(primlist->lock);

	render_obj = get_idle_qobj(render_queue);
	render_obj->data1 = primlist;
	qobj_set_ready(render_obj);

#ifdef USE_THREAD_RENDER
	osd_event_set(render_event);
#else
	do_render();
#endif

	vt_update_key();

}

#ifdef USE_THREAD_RENDER

static void *video_fbcon_render_thread(void *param)
{

	while(1){
		osd_event_wait(render_event, OSD_EVENT_WAIT_INFINITE);
		if(osdmini_run==0)
			break;

		do_render();
	}

	printk("video_fbcon_render_thread stop!\n");
	return NULL;
}

static void *video_fbcon_vblank_thread(void *param)
{
	QOBJ *new_obj;
	QOBJ *release_obj = NULL;

	while(1){
		new_obj = get_ready_qobj(fbo_queue);
		if(new_obj){
			video_swap_buffer(new_obj);
			//printk("display: %p\n", new_obj);
			if(release_obj){
				//printf("release: %p\n", release_obj);
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

	printk("video_fbcon_vblank_thread stop!\n");
	return NULL;
}


#endif


/******************************************************************************/


void video_init_fbcon(void)
{
	signal(SIGINT, sigint_handle);

	vt_init();

	fb_init();
}

void video_exit_fbcon(void)
{
	printk("video_exit_fbcon!\n");
	vt_exit();
}


/******************************************************************************/

//============================================================
//  SOFTWARE RENDERING
//============================================================

#define FUNC_PREFIX(x)      rgb888_##x
#define PIXEL_TYPE          UINT32
#define SRCSHIFT_R          0
#define SRCSHIFT_G          0
#define SRCSHIFT_B          0
#define DSTSHIFT_R          16
#define DSTSHIFT_G          8
#define DSTSHIFT_B          0

#include "rendersw.c"

