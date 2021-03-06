INLINE UINT32 FETCH_TEXEL_RGBA4_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA4_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA4_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA8_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA8_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA8_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA16_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA16_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA16_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA32_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA32_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_RGBA32_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_YUV16(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI4_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI4_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI4_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI8_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI8_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI8_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI16_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI16_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_CI16_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA4_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA4_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA4_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA8_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA8_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA8_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA16_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA16_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_IA16_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_I4_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_I4_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_I4_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_I8_TLUT_EN0(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_I8_TLUT_EN1(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_I8_TLUT_NEN(UINT32 s, UINT32 t);
INLINE UINT32 FETCH_TEXEL_INVALID(UINT32 s, UINT32 t);

static UINT32 (*rdp_fetch_texel_func[128])(UINT32, UINT32) =
{
	// 4-bit accessors
	FETCH_TEXEL_RGBA4_TLUT_NEN, FETCH_TEXEL_RGBA4_TLUT_NEN, FETCH_TEXEL_RGBA4_TLUT_EN0, FETCH_TEXEL_RGBA4_TLUT_EN1,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_CI4_TLUT_NEN,	FETCH_TEXEL_CI4_TLUT_NEN,	FETCH_TEXEL_CI4_TLUT_EN0,	FETCH_TEXEL_CI4_TLUT_EN1,
	FETCH_TEXEL_IA4_TLUT_NEN,	FETCH_TEXEL_IA4_TLUT_NEN,	FETCH_TEXEL_IA4_TLUT_EN0,	FETCH_TEXEL_IA4_TLUT_EN1,
	FETCH_TEXEL_I4_TLUT_NEN,	FETCH_TEXEL_I4_TLUT_NEN,	FETCH_TEXEL_I4_TLUT_EN0,	FETCH_TEXEL_I4_TLUT_EN1,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,

	// 8-bit accessors
	FETCH_TEXEL_RGBA8_TLUT_NEN, FETCH_TEXEL_RGBA8_TLUT_NEN, FETCH_TEXEL_RGBA8_TLUT_EN0, FETCH_TEXEL_RGBA8_TLUT_EN1,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_CI8_TLUT_NEN,	FETCH_TEXEL_CI8_TLUT_NEN,	FETCH_TEXEL_CI8_TLUT_EN0,	FETCH_TEXEL_CI8_TLUT_EN1,
	FETCH_TEXEL_IA8_TLUT_NEN,	FETCH_TEXEL_IA8_TLUT_NEN,	FETCH_TEXEL_IA8_TLUT_EN0,	FETCH_TEXEL_IA8_TLUT_EN1,
	FETCH_TEXEL_I8_TLUT_NEN,	FETCH_TEXEL_I8_TLUT_NEN,	FETCH_TEXEL_I8_TLUT_EN0,	FETCH_TEXEL_I8_TLUT_EN1,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,

	// 16-bit accessors
	FETCH_TEXEL_RGBA16_TLUT_NEN,FETCH_TEXEL_RGBA16_TLUT_NEN,FETCH_TEXEL_RGBA16_TLUT_EN0,FETCH_TEXEL_RGBA16_TLUT_EN1,
	FETCH_TEXEL_YUV16,			FETCH_TEXEL_YUV16,			FETCH_TEXEL_YUV16,			FETCH_TEXEL_YUV16,
	FETCH_TEXEL_CI16_TLUT_NEN,	FETCH_TEXEL_CI16_TLUT_NEN,	FETCH_TEXEL_CI16_TLUT_EN0,	FETCH_TEXEL_CI16_TLUT_EN1,
	FETCH_TEXEL_IA16_TLUT_NEN,	FETCH_TEXEL_IA16_TLUT_NEN,	FETCH_TEXEL_IA16_TLUT_EN0,	FETCH_TEXEL_IA16_TLUT_EN1,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,

	// 32-bit accessors
	FETCH_TEXEL_RGBA32_TLUT_NEN,FETCH_TEXEL_RGBA32_TLUT_NEN,FETCH_TEXEL_RGBA32_TLUT_EN0,FETCH_TEXEL_RGBA32_TLUT_EN1,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
	FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,		FETCH_TEXEL_INVALID,
};
