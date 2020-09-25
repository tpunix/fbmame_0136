


.text


// void m_resize_h_neon64(uint8_t *dst, int dw, uint8_t *src, int fx, int dx)
.global m_resize_h_neon64
m_resize_h_neon64:
	ins		V5.S[0], W3
	add		W3, W3, W4
	dup		V1.4S, W4
	ins		V5.S[1], W3
	add		W3, W3, W4
	adr		X5, _cf_table
	ins		V5.S[2], W3
	add		W3, W3, W4
	shl		V6.4S, V1.4S, 2  // V6: [4dx, 4dx, 4dx, 4dx]
	ins		V5.S[3], W3      // V5: [fx+3*dx, fx+2*dx, fx+1*dx, fx+0*dx]
	ld1		{V3.16B}, [X5]
	movi	V4.16B, 0xff
1:
	umov	W3, V5.H[1]
	umov	W4, V5.H[3]
	add		X3, X2, X3, lsl 2
	add		X4, X2, X4, lsl 2
	ld2		{V0.S, V1.S}[0], [X3]
	ld2		{V0.S, V1.S}[1], [X4]
	umov	W5, V5.H[5]
	umov	W6, V5.H[7]
	add		X5, X2, X5, lsl 2
	add		X6, X2, X6, lsl 2
	ld2		{V0.S, V1.S}[2], [X5]
	ld2		{V0.S, V1.S}[3], [X6]

	tbl		V2.16B, {V5.16B}, V3.16B
	eor		V7.16B, V2.16B, V4.16B

	umull	V16.8H, V1.8B, V2.8B
	umull2	V17.8H, V1.16B, V2.16B
	add		V5.4S, V5.4S, V6.4S
	umlal	V16.8H, V0.8B, V7.8B
	umlal2	V17.8H, V0.16B, V7.16B
	subs	W1, W1, 4
	st1		{V16.8H, V17.8H}, [X0], 32

	bgt		1b

	ret

_cf_table:
	.byte 1,1,1,1, 5,5,5,5, 9,9,9,9, 13,13,13,13



// void m_resize_v(uint8_t *dst, int dw, uint8_t *row0, uint8_t *row1, int cf0, int cf1)
.global m_resize_v_neon64
m_resize_v_neon64:
	dup		V3.8H, W5
	adr		X5, _px_table
	dup		V2.8H, W4
	ld1		{V6.8B}, [X5]
1:
	ld1		{V0.8H}, [X2], 16
	ld1		{V1.8H}, [X3], 16

	umull	V4.4S, V0.4H, V2.4H
	umull2	V5.4S, V0.8H, V2.8H
	subs	W1, W1, 2
	umlal	V4.4S, V1.4H, V3.4H
	umlal2	V5.4S, V1.8H, V3.8H

	tbl		V4.8B, {V4.16B, V5.16B}, V6.8B
	st1		{V4.8B}, [X0], 8
	bgt		1b

	ret

_px_table:
	.byte 3,7,11,15, 19,23,27,31











