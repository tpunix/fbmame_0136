// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  video.c - Video function for mini OSD
//
//============================================================

#include "osdmini.h"


#include <mqueue.h>
#include <errno.h>


/******************************************************************************/


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


QOBJ *get_idle_qobj(SIMPLE_QUEUE *sq)
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

QOBJ *get_ready_qobj(SIMPLE_QUEUE *sq)
{
	int i, r, age;

	r = -1;
	age = 0x7fffffff;

	for(i=0; i<sq->size; i++){
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


void qobj_set_ready(QOBJ *qobj)
{
	qobj->age = qobj_ages++;
	qobj->state = QOBJ_READY;
}

void qobj_set_idle(QOBJ *qobj)
{
	qobj->age = 0;
	qobj->state = QOBJ_IDLE;
}

void qobj_init(QOBJ *qobj, void *data1, int data2)
{
	qobj->state = QOBJ_IDLE;
	qobj->age = 0;
	qobj->data1 = data1;
	qobj->data2 = data2;
}


/******************************************************************************/


int fb_xres, fb_yres, fb_bpp, fb_pitch;
UINT8 *fb_base_addr;

SIMPLE_QUEUE *fbo_queue;
mqd_t render_mq;

static osd_thread *render_thread;
static void *video_render_thread(void *param);
static int video_running;


void (*osd_video_init_backend)(void) = NULL;
void (*osd_video_exit_backend)(void) = NULL;


void osd_video_init(void)
{
	struct mq_attr mattr;

	g_pause = 0;

	osd_video_init_backend();

	mattr.mq_flags = 0;
	mattr.mq_maxmsg = 4;
	mattr.mq_msgsize = sizeof(void*);
	mattr.mq_curmsgs = 0;
	render_mq = mq_open("/mq_render", O_RDWR|O_CREAT|O_EXCL, 0666, &mattr);
	if(render_mq<0 && errno==EEXIST){
		mq_unlink("/mq_render");
		render_mq = mq_open("/mq_render", O_RDWR|O_CREAT|O_EXCL, 0666, &mattr);
	}
	if(render_mq<0){
		perror("mq_create(mq_render)");
		exit(-1);
	}

	video_running = 1;
	render_thread = osd_thread_create(video_render_thread, NULL);
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

int g_pause;
float aspect_set;
static float game_aspect=0;
static int game_width=0, game_height=0;
static int fb_draw_w=0, fb_draw_h=0;
static int fb_draw_offset = 0;

static void dump_primlist(render_primitive_list *primlist)
{
	const render_primitive *prim = primlist->head;
	int type;

	while(prim){
		if(prim->type==RENDER_PRIMITIVE_LINE){
			printf("LINE: ");
			type = 1;
		}else if(prim->texture.base){
			printf("QUAD: ");
			type = 2;
		}else{
			printf("RECT: ");
			type = 3;
		}

		printf("%08x (%4d,%4d), (%4d,%4d), [%3d %3d %3d %3d]\n", prim->flags,
				(int)prim->bounds.x0, (int)prim->bounds.y0, (int)prim->bounds.x1, (int)prim->bounds.y1,
				(int)(prim->color.r*255), (int)(prim->color.g*255), (int)(prim->color.b*255), (int)(prim->color.a*255));
		if(type==2){
			printf("    texture: (%4d,%4d), pal:%p\n",
				prim->texture.width, prim->texture.height, prim->texture.palette);
			printf("    text_uv: (%4d,%4d), (%4d,%4d), (%4d,%4d), (%4d,%4d)\n",
				(int)prim->texcoords.tl.u, (int)prim->texcoords.tl.v,
				(int)prim->texcoords.tr.u, (int)prim->texcoords.tr.v,
				(int)prim->texcoords.bl.u, (int)prim->texcoords.bl.v,
				(int)prim->texcoords.br.u, (int)prim->texcoords.br.v
				);
		}

		prim = prim->next;
	}

	printf("\n");

}

#ifdef USE_MAME_RENDER
static void rgb888_draw_primitives(const render_primitive *prim, void *dstdata, UINT32 width, UINT32 height, UINT32 pitch);
#else
void draw_primlist(render_primitive_list *primlist, UINT8 *dst_addr, int width, int height, int pitch);
#endif

static void do_render(render_primitive_list *primlist)
{
	QOBJ *draw_obj;
	UINT8 *fb_draw_ptr;

	//dump_primlist(primlist);
	//video_show_fps();

	if(g_pause==0){
		draw_obj = get_idle_qobj(fbo_queue);
		if(draw_obj){
			fb_draw_ptr = (UINT8*)(draw_obj->data1) + fb_draw_offset;
			// do the drawing here
#ifdef USE_MAME_RENDER
			rgb888_draw_primitives(primlist->head, fb_draw_ptr, fb_draw_w, fb_draw_h, fb_pitch/4);
#else
			draw_primlist(primlist, fb_draw_ptr, fb_draw_w, fb_draw_h, fb_pitch);
#endif
			qobj_set_ready(draw_obj);
		}
	}

	osd_lock_release(primlist->lock);
	render_target_free_primitives(primlist);

}


static void *video_render_thread(void *param)
{
	int retv=0;
	render_primitive_list *primlist;

	while(video_running){
		retv = mq_receive(render_mq, (char*)&primlist, sizeof(void*), NULL);
		if(retv<0){
			if(errno==EINTR || errno==EAGAIN)
				continue;
			break;
		}

		if(video_running==0)
			break;
		do_render(primlist);
	}
	if(retv<0)
		perror("mq_receive");

	video_running = -1;
	printk("video_render_thread stop!\n");
	return NULL;
}


/******************************************************************************/


void osd_video_update(int skip_draw)
{
	if(skip_draw){
		return;
	}

	// get the minimum width/height for the current layout
	int minwidth, minheight;
	render_target_get_minimum_size(our_target, &minwidth, &minheight);

	float new_aspect;
	if(aspect_set){
		if(aspect_set==1111){
			aspect_set = (float)minwidth/(float)minheight;
		}else if(aspect_set==2222){
			aspect_set = (float)fb_xres/(float)fb_yres;
		}
		new_aspect = aspect_set;
	}else{
		new_aspect = render_get_effective_aspect(our_target);
	}

	if(game_width!=minwidth || game_height!=minheight || game_aspect != new_aspect){
		printk("Change res to %dx%d\n", minwidth, minheight);
		game_width = minwidth;
		game_height = minheight;
		game_aspect = new_aspect;

#if 0
		fb_draw_w = (minwidth * fb_yres) / minheight;
		if(fb_draw_w<fb_xres){
			fb_draw_h = fb_yres;
			fb_draw_offset = ((fb_xres-fb_draw_w)/2)*(fb_bpp/8);
		}else{
			fb_draw_h = (minheight * fb_xres) / minwidth;
			fb_draw_w = fb_xres;
			fb_draw_offset = ((fb_yres-fb_draw_h)/2)*fb_pitch;
		}
#else
		float fb_aspect = (float)fb_xres/(float)fb_yres;
		if(new_aspect>fb_aspect){
			fb_draw_w = fb_xres;
			fb_draw_h = (float)fb_xres/new_aspect;
			fb_draw_offset = ((fb_yres-fb_draw_h)/2)*fb_pitch;
		}else{
			fb_draw_w = (float)fb_yres*new_aspect;
			fb_draw_h = fb_yres;
			fb_draw_offset = ((fb_xres-fb_draw_w)/2)*(fb_bpp/8);
		}
#endif
		printk("Scale: %dx%d\n", fb_draw_w, fb_draw_h);

	}
	render_target_set_bounds(our_target, fb_draw_w, fb_draw_h, 0);

	// get the list of primitives for the target at the current size
	const render_primitive_list *primlist = render_target_get_primitives(our_target);
	osd_lock_acquire(primlist->lock);

	mq_send(render_mq, (char*)&primlist, sizeof(void*), 0);
}


void osd_video_exit(void)
{
	printk("osd_video_exit!\n");
	char *empty = NULL;

	video_running = 0;

	osd_sleep(100000);
	mq_send(render_mq, (char*)&empty, sizeof(char*), 0);

	while(video_running!=-1){
		osd_sleep(1000);
	}

	mq_close(render_mq);
	mq_unlink("/mq_render");

	osd_video_exit_backend();
}


/******************************************************************************/

#ifdef USE_MAME_RENDER

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

#endif

