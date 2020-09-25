// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  minimain.c - Main function for mini OSD
//
//============================================================

#include "osdepend.h"
#include "render.h"
#include "clifront.h"
#include "osdcore.h"
#include "osdmini.h"


#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <errno.h>


/******************************************************************************/

#ifdef __aarch64__
#define USE_NEON
#include <arm_neon.h>
#else
#define USE_SSE4
#include <smmintrin.h>
#endif



#ifdef USE_SSE4


static UINT8 shufb_alpha[16] =
{
	0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80,
	0x07, 0x80, 0x07, 0x80, 0x07, 0x80, 0x07, 0x80,
};

static UINT8 shufb_h2b[16] =
{
	0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

static inline __m128i _mm_load_i64(void *addr)
{
	__m128i ret;
	__asm volatile ("movq (%1), %q0" : "=x" (ret) : "r" (addr));
	return ret;
}

void blend_line_sample_sse(UINT8 *dst, UINT8 *src, int width)
{

	__m128i ext_alpha = _mm_load_si128((__m128i*)shufb_alpha);
	__m128i to_pixel  = _mm_load_si128((__m128i*)shufb_h2b);
	__m128i c_00ff = _mm_set1_epi16(0x00ff);

	while(width>0){
		__m128i msrc = _mm_load_i64(src);
		__m128i mdst = _mm_load_i64(dst);
		__m128i msa = _mm_shuffle_epi8(msrc, ext_alpha);
		msrc = _mm_cvtepu8_epi16(msrc);
		mdst = _mm_cvtepu8_epi16(mdst);
		__m128i msum = _mm_mullo_epi16(msrc, msa) + _mm_mullo_epi16(mdst, c_00ff^msa);
		__m128i md = _mm_shuffle_epi8(msum, to_pixel);
		*(UINT64*)dst = _mm_cvtsi128_si64(md);

		src += 8;
		dst += 8;
		width -= 2;
	}
}


#endif

#ifdef USE_NEON
void blend_line_sample_neon(UINT8 *dst, UINT8 *src, int width)
{
	__builtin_prefetch(src);
	__builtin_prefetch(dst);

	uint8x8_t c_ff = vdup_n_u8(0xff);

	while(width>0){
		__builtin_prefetch(src+32);
		__builtin_prefetch(dst+32);
		uint8x8x4_t mfg = vld4_u8(src);
		uint8x8x4_t mbg = vld4_u8(dst);

		mfg.val[2] = vmovn_u16(vshrq_n_u16(vaddq_u16(vmull_u8(mfg.val[2], mfg.val[3]), vmull_u8(mbg.val[2], vmvn_u8(mfg.val[3]))),8));
		mfg.val[1] = vmovn_u16(vshrq_n_u16(vaddq_u16(vmull_u8(mfg.val[1], mfg.val[3]), vmull_u8(mbg.val[1], vmvn_u8(mfg.val[3]))),8));
		mfg.val[0] = vmovn_u16(vshrq_n_u16(vaddq_u16(vmull_u8(mfg.val[0], mfg.val[3]), vmull_u8(mbg.val[0], vmvn_u8(mfg.val[3]))),8));
		mfg.val[3] = c_ff;

		vst4_u8(dst, mfg);

		src += 8*4;
		dst += 8*4;
		width -= 8;
	}
}

#endif


void blend_line_sample_c(UINT32 *dst, UINT32 *src, int width)
{
	int x;

	for(x=0; x<width; x++){
		UINT32 sp = *src++;
		UINT32 sa = (sp>>24)&0xff;
		UINT32 sr = (sp>>16)&0xff;
		UINT32 sg = (sp>> 8)&0xff;
		UINT32 sb = (sp>> 0)&0xff;
		if(sa){
			UINT32 dp = *dst;
			UINT32 dr = (dp>>16)&0xff;
			UINT32 dg = (dp>> 8)&0xff;
			UINT32 db = (dp>> 0)&0xff;
			UINT32 da = 0x100-sa;

			dr = (sr*sa + dr*da)>>8;
			dg = (sg*sa + dg*da)>>8;
			db = (sb*sa + db*da)>>8;

			*dst = 0xff000000 | (dr<<16) | (dg<<8) | db;
		}
		dst++;
	}
}


void blend_line_sample_c1(UINT8 *dst, UINT8 *src, int width)
{
	UINT32 sp, dp, sa, da;
	UINT32 srb, sag, drb, dag;

	while(width>0){
		sp = *(UINT32*)src;
		sa = sp>>24;
		if(sa){
			dp = *(UINT32*)dst;
			da = sa^0xff;
			srb = sp & 0x00ff00ff; // __sr__sb
			sag = sp & 0x0000ff00; // ____sg__
			drb = dp & 0x00ff00ff; // __dr__db
			dag = dp & 0x0000ff00; // ____dg__
			drb = (srb*sa + drb*da)>>8;     // __drrrdb
			dag = (sag*sa + dag*da)>>8;     // ____dggg
			dp = 0xff000000 | (drb&0x00ff00ff) | (dag&0x0000ff00);
			*(UINT32*)dst = dp;
		}

		src += 4;
		dst += 4;
		width -= 1;
	}
}


void blend_line_sample_c2(UINT8 *dst, UINT8 *src, int width)
{
	UINT32 sa, da;

	while(width>0){
		sa = src[3];
		if(sa){
			UINT32 da = 0x100-sa;
			dst[3] = 0xff;
			dst[2] = (src[2]*sa + dst[2]*da)>>8;
			dst[1] = (src[1]*sa + dst[1]*da)>>8;
			dst[0] = (src[0]*sa + dst[0]*da)>>8;
		}
		src += 4;
		dst += 4;
		width -= 1;
	}
}


/******************************************************************************/

void m_resize_rgba_c(UINT8* dst, int dst_width, int dst_height, int dst_stride,
					 UINT8* src, int src_width, int src_height, int src_stride, UINT32 *palette, int scanline);


static UINT8 srcbuf[0x10000];

static void draw_quad_rgb32(const render_primitive *prim, UINT8 *dst_addr, int width, int height, int pitch)
{
	int x0 = prim->bounds.x0;
	int y0 = prim->bounds.y0;
	int x1 = prim->bounds.x1;
	int y1 = prim->bounds.y1;
	int tw = prim->texture.width;
	int th = prim->texture.height;
	int w = x1-x0;
	int h = y1-y0;
	int x, y;



	UINT32 *src_addr = (UINT32*)prim->texture.base;
	dst_addr += y0*pitch + x0*4;

	int format = PRIMFLAG_GET_TEXFORMAT(prim->flags);
	int mode = PRIMFLAG_GET_BLENDMODE(prim->flags);

	int scanline = (prim->flags&0x2000)? g_scanline_mode : 0;


	if(format==TEXFORMAT_RGB32 || (format==TEXFORMAT_ARGB32 && mode==BLENDMODE_NONE)){
		m_resize_rgba_c(
			dst_addr, w, h, pitch,
			(UINT8*)prim->texture.base, tw, th, prim->texture.rowpixels*4, NULL, scanline);
	}else if(format==TEXFORMAT_PALETTE16){
		m_resize_rgba_c(
			dst_addr, w, h, pitch,
			(UINT8*)prim->texture.base, tw, th, prim->texture.rowpixels*2, (UINT32*)prim->texture.palette, scanline);
	}else if(format==TEXFORMAT_ARGB32 && mode==BLENDMODE_ALPHA){
		UINT32 pr = 256.0f * prim->color.r;
		UINT32 pg = 256.0f * prim->color.g;
		UINT32 pb = 256.0f * prim->color.b;
		UINT32 pa = 256.0f * prim->color.a;
		int src_rowpixels = prim->texture.rowpixels;

#if 1
		if(w!=tw || h!=th){
			int new_pitch = ((w+7)&~7)*4;
			m_resize_rgba_c(
				srcbuf, w, h, new_pitch,
				(UINT8*)prim->texture.base, tw, th, prim->texture.rowpixels*4, NULL, 0);
			src_addr = (UINT32*)srcbuf;
			src_rowpixels = new_pitch/4;
		}
#else
		if(w>tw) w = tw;
		if(th>1){
			if(h>th)
				h = th;
		}else{
			src_rowpixels = 0;
		}
#endif

		if(pr>=256 && pg>=256 && pb>=256 && pa>=256){
			// simple mode, no color
			for(y=0; y<h; y++){
				//blend_line_sample_c((UINT32*)dst_addr, (UINT32*)src_addr, w);
				blend_line_sample_c1((UINT8*)dst_addr, (UINT8*)src_addr, w);
				//blend_line_sample_c2((UINT8*)dst_addr, (UINT8*)src_addr, w);
				//blend_line_sample_sse((UINT8*)dst_addr, (UINT8*)src_addr, w);
				//blend_line_sample_neon((UINT8*)dst_addr, (UINT8*)src_addr, w);
				dst_addr += pitch;
				src_addr += src_rowpixels;
			}

		}else{

			for(y=0; y<h; y++){
				UINT32 *dst = (UINT32*)dst_addr;
				UINT32 *src = (UINT32*)src_addr;
				for(x=0; x<w; x++){
					UINT32 sp = *src++;
					UINT32 sa = (sp>>24)&0xff;
					UINT32 sr = (sp>>16)&0xff;
					UINT32 sg = (sp>> 8)&0xff;
					UINT32 sb = (sp>> 0)&0xff;
					if(sa){
						sa *= pa;
						UINT32 dp = *dst;
						UINT32 dr = (dp>>16)&0xff;
						UINT32 dg = (dp>> 8)&0xff;
						UINT32 db = (dp>> 0)&0xff;
						UINT32 da = 0x10000-sa;

						dr = (sr*pr*sa + dr*da)>>24;
						dg = (sg*pg*sa + dg*da)>>24;
						db = (sb*pb*sa + db*da)>>24;

						*dst++ = 0xff000000 | (dr<<16) | (dg<<8) | db;
					}else{
						dst++;
					}
				}
				dst_addr += pitch;
				src_addr += src_rowpixels;
			}
		}
	}

}


/******************************************************************************/


static void draw_line_rgb32(const render_primitive *prim, UINT8 *dst_addr, int width, int height, int pitch)
{
	int x0 = prim->bounds.x0;
	int y0 = prim->bounds.y0;
	int x1 = prim->bounds.x1;
	int y1 = prim->bounds.y1;
	int x, y;

	int r = 255.0f * prim->color.r * prim->color.a;
	int g = 255.0f * prim->color.g * prim->color.a;
	int b = 255.0f * prim->color.b * prim->color.a;
	int color = 0xff000000 | (r<<16) | (g<<8) | b;

	if(x0==x1){
		if(y0>y1){ y=y0; y0=y1; y1=y;}
		dst_addr += y0*pitch + x0*4;
		for(y=y0; y<=y1; y++){
			*(UINT32*)(dst_addr) = color;
			dst_addr += pitch;
		}
	}else if(y0==y1){
		if(x0>x1){ x=x0; x0=x1; x1=x;}
		dst_addr += y0*pitch + x0*4;
		for(x=x0; x<=x1; x++){
			*(UINT32*)(dst_addr) = color;
			dst_addr += 4;
		}
	}else{
		printk("LINE: (%d,%d)-(%d,%d) %08x\n", x0, y0, x1, y1, color);
	}

}


static void draw_rect_rgb32(const render_primitive *prim, UINT8 *dst_addr, int width, int height, int pitch)
{
	int x0 = prim->bounds.x0;
	int y0 = prim->bounds.y0;
	int x1 = prim->bounds.x1;
	int y1 = prim->bounds.y1;
	int x, y;

	UINT32 r = 255.0f * prim->color.r;
	UINT32 g = 255.0f * prim->color.g;
	UINT32 b = 255.0f * prim->color.b;
	UINT32 a = 256.0f * prim->color.a;

	UINT32 *dst;
	dst_addr += y0*pitch + x0*4;

	if( (PRIMFLAG_GET_BLENDMODE(prim->flags) == BLENDMODE_NONE) || a>=256){
		UINT32 color = 0xff000000 | (r<<16) | (g<<8) | b;

		for(y=y0; y<y1; y++){
			dst = (UINT32*)dst_addr;
			for(x=x0; x<x1; x++){
				*dst++ = color;
			}
			dst_addr += pitch;
		}

	}else if(a>0){
		r = (r*a)<<16;
		g = (g*a)<<8;
		b = (b*a)<<0;
		UINT32 inva = 256-a;

		for(y=y0; y<y1; y++){
			dst = (UINT32*)dst_addr;
			for(x=x0; x<x1; x++){
				UINT32 dp = *dst;
				UINT32 dr = (r + (dp&0x00ff0000)*inva)>>24;
				UINT32 dg = (g + (dp&0x0000ff00)*inva)>>16;
				UINT32 db = (b + (dp&0x000000ff)*inva)>>8;
				*dst++ = 0xff000000 | (dr<<16) | (dg<<8) | db;
			}
			dst_addr += pitch;
		}
	}

}

/******************************************************************************/

void draw_primlist(render_primitive_list *primlist, UINT8 *dst_addr, int width, int height, int pitch)
{
	const render_primitive *prim = primlist->head;

	while(prim){
		if(prim->type==RENDER_PRIMITIVE_LINE){
			draw_line_rgb32(prim, dst_addr, width, height, pitch);
		}else if(prim->texture.base==NULL){
			draw_rect_rgb32(prim, dst_addr, width, height, pitch);
		}else{
			draw_quad_rgb32(prim, dst_addr, width, height, pitch);
		}

		prim = prim->next;
	}

}


/******************************************************************************/


