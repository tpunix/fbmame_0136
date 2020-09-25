
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define COEF_BITS 16
#define COEF_MASK ((1<<COEF_BITS)-1)


// 两个行缓存. 最大值: 1920x4x2 = 15360
static uint8_t row_buf0[0x4000];
static uint8_t row_buf1[0x4000];

// 两个src缓存. 最大值: 1920x4 = 7680
// 仅用于palette格式的src
static uint8_t src_buf0[0x2000];
static uint8_t src_buf1[0x2000];


#if defined(__aarch64__)

void m_resize_h_neon64(uint8_t *dst, int dw, uint8_t *src, int fx, int dx);
void m_resize_v_neon64(uint8_t *dst, int dw, uint8_t *row0, uint8_t *row1, int cf0, int cf1);

#define RESIZE_H  m_resize_h_neon64
#define RESIZE_V  m_resize_v_neon64

#else

static void m_resize_h(uint8_t *dst, int dw, uint8_t *src, int fx, int dx)
{
	int sx, cf0, cf1;
	uint8_t *sp;
	uint16_t *dp = (uint16_t*)dst;

	while(dw>0){
		cf1 = (fx>>8)&0xff;
		cf0 = cf1^0xff;
		sx = fx>>16;
		sp = src+sx*4;

		dp[0] = sp[0]*cf0 + sp[4]*cf1;
		dp[1] = sp[1]*cf0 + sp[5]*cf1;
		dp[2] = sp[2]*cf0 + sp[6]*cf1;
		dp[3] = sp[3]*cf0 + sp[7]*cf1;

		dp += 4;
		fx += dx;
		dw -= 1;

		cf1 = (fx>>8)&0xff;
		cf0 = cf1^0xff;
		sx = fx>>16;
		sp = src+sx*4;

		dp[0] = sp[0]*cf0 + sp[4]*cf1;
		dp[1] = sp[1]*cf0 + sp[5]*cf1;
		dp[2] = sp[2]*cf0 + sp[6]*cf1;
		dp[3] = sp[3]*cf0 + sp[7]*cf1;

		dp += 4;
		fx += dx;
		dw -= 1;
	}

}


static void m_resize_v(uint8_t *dst, int dw, uint8_t *row0, uint8_t *row1, int cf0, int cf1)
{
	uint16_t *s0 = (uint16_t*)row0;
	uint16_t *s1 = (uint16_t*)row1;

	while(dw>0){
		dst[0] = (s0[0]*cf0 + s1[0]*cf1)>>24;
		dst[1] = (s0[1]*cf0 + s1[1]*cf1)>>24;
		dst[2] = (s0[2]*cf0 + s1[2]*cf1)>>24;
		dst[3] = (s0[3]*cf0 + s1[3]*cf1)>>24;

		s0 += 4;
		s1 += 4;
		dst += 4;
		dw -= 1;
	}

}


#define RESIZE_H  m_resize_h
#define RESIZE_V  m_resize_v

#endif


static uint8_t *pal_to_rgba(uint8_t *dst, uint8_t *src, int width, uint32_t *palette)
{
	uint32_t *pd = (uint32_t*)dst;
	uint16_t *ps = (uint16_t*)src;

	while(width>=4){
		*pd++ = palette[*ps++];
		*pd++ = palette[*ps++];
		*pd++ = palette[*ps++];
		*pd++ = palette[*ps++];
		width -= 4;
	}
	while(width>0){
		*pd++ = palette[*ps++];
		width -= 1;
	}

	return dst;
}


void m_resize_rgba_c(uint8_t* dst, int dw, int dh, int dstride, uint8_t* src, int sw, int sh, int sstride, uint32_t *palette, int scanline)
{
	int y, fx, fy, cf0, cf1, step_y, step_x;
	int ky, k0, k1;
	uint8_t *rows[2];
	uint8_t *srows[2];

	step_y = ((sh-1)<<COEF_BITS)/(dh-1);
	step_x = ((sw-1)<<COEF_BITS)/(dw-1);

	fx = 0;
	if(dw*2==sw)
		fx = 1<<(COEF_BITS-1);
	fy = 0;
	if(dh*2==sh)
		fy = 1<<(COEF_BITS-1);

	k0 = -1; // src缓存行0
	k1 = -1; // src缓存行1
	// dst行在k0与k1之间插值产生
	rows[0] = row_buf0;
	rows[1] = row_buf1;


	for(y=0; y<dh; y++){
		cf1 = fy&COEF_MASK;
		cf0 = COEF_MASK-cf1;
		ky = fy>>COEF_BITS; // 当前行对应的src行

		if(ky>k1){
			// 需要新的两个src缓存行
			k0 = ky;
			k1 = ky+1;
			if(palette){
				srows[0] = pal_to_rgba(src_buf0, src+k0*sstride, sw, palette);
				srows[1] = pal_to_rgba(src_buf1, src+k1*sstride, sw, palette);
			}else{
				srows[0] = src + k0*sstride;
				srows[1] = src + k1*sstride;
			}
			if(k1>=sh) // sh可能为1
				srows[1] = srows[0];
			RESIZE_H(rows[0], dw, srows[0], fx, step_x);
			RESIZE_H(rows[1], dw, srows[1], fx, step_x);
		}else if(ky==k1){
			// 需要一个新的缓存行: (k1, new) -> (k0, k1)
			uint8_t *tmp = rows[1];
			rows[1] = rows[0];
			rows[0] = tmp;
			k0 = k1;
			k1 = ky+1;
			if(palette){
				srows[1] = pal_to_rgba(src_buf1, src+k1*sstride, sw, palette);
			}else{
				srows[1] = src + k1*sstride;
			}
			RESIZE_H(rows[1], dw, srows[1], fx, step_x);
		}else{
			// 不需要更新缓存行
		}

		//printf("Line %3d: src=%3d  k0=%3d k1=%3d\n", y, ky, k0, k1);
		if(scanline){
			if((y%scanline)==(scanline-1)){
				cf0 >>= 1;
				cf1 >>= 1;
			}
			RESIZE_V(dst, dw, rows[0], rows[1], cf0, cf1);
		}else{
			RESIZE_V(dst, dw, rows[0], rows[1], cf0, cf1);
		}

		dst += dstride;
		fy += step_y;  // 下一行
	}

}


