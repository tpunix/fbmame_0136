static void texture_rectangle_16bit_c1_nzc_nzu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_nzc_nzu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_nzc_nzu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_nzc_nzu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_nzc_nzu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_nzc_nzu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_nzc_nzu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_nzc_nzu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_nzc_nzu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_nzc_nzu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_nzc_nzu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_nzc_nzu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_zc_nzu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_zc_nzu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_zc_nzu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_zc_nzu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_zc_nzu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_zc_nzu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_zc_nzu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_zc_nzu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_zc_nzu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_zc_nzu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_zc_nzu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_zc_nzu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_nzc_zu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_nzc_zu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_nzc_zu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_nzc_zu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_nzc_zu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_nzc_zu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_nzc_zu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_nzc_zu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_nzc_zu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_nzc_zu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_nzc_zu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_nzc_zu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_zc_zu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_zc_zu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c1_zc_zu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_zc_zu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_zc_zu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_c2_zc_zu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_zc_zu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_zc_zu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cc_zc_zu_dn(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_zc_zu_dm(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_zc_zu_db(TEX_RECTANGLE *rect);
static void texture_rectangle_16bit_cf_zc_zu_dn(TEX_RECTANGLE *rect);

static void (*rdp_texture_rectangle_16bit_func[64])(TEX_RECTANGLE *) =
{
	texture_rectangle_16bit_c1_nzc_nzu_dm, texture_rectangle_16bit_c1_nzc_nzu_db, texture_rectangle_16bit_c1_nzc_nzu_dn, texture_rectangle_16bit_c1_nzc_nzu_dn,
	texture_rectangle_16bit_c2_nzc_nzu_dm, texture_rectangle_16bit_c2_nzc_nzu_db, texture_rectangle_16bit_c2_nzc_nzu_dn, texture_rectangle_16bit_c2_nzc_nzu_dn,
	texture_rectangle_16bit_cc_nzc_nzu_dm, texture_rectangle_16bit_cc_nzc_nzu_db, texture_rectangle_16bit_cc_nzc_nzu_dn, texture_rectangle_16bit_cc_nzc_nzu_dn,
	texture_rectangle_16bit_cf_nzc_nzu_dm, texture_rectangle_16bit_cf_nzc_nzu_db, texture_rectangle_16bit_cf_nzc_nzu_dn, texture_rectangle_16bit_cf_nzc_nzu_dn,
	texture_rectangle_16bit_c1_zc_nzu_dm, texture_rectangle_16bit_c1_zc_nzu_db, texture_rectangle_16bit_c1_zc_nzu_dn, texture_rectangle_16bit_c1_zc_nzu_dn,
	texture_rectangle_16bit_c2_zc_nzu_dm, texture_rectangle_16bit_c2_zc_nzu_db, texture_rectangle_16bit_c2_zc_nzu_dn, texture_rectangle_16bit_c2_zc_nzu_dn,
	texture_rectangle_16bit_cc_zc_nzu_dm, texture_rectangle_16bit_cc_zc_nzu_db, texture_rectangle_16bit_cc_zc_nzu_dn, texture_rectangle_16bit_cc_zc_nzu_dn,
	texture_rectangle_16bit_cf_zc_nzu_dm, texture_rectangle_16bit_cf_zc_nzu_db, texture_rectangle_16bit_cf_zc_nzu_dn, texture_rectangle_16bit_cf_zc_nzu_dn,
	texture_rectangle_16bit_c1_nzc_zu_dm, texture_rectangle_16bit_c1_nzc_zu_db, texture_rectangle_16bit_c1_nzc_zu_dn, texture_rectangle_16bit_c1_nzc_zu_dn,
	texture_rectangle_16bit_c2_nzc_zu_dm, texture_rectangle_16bit_c2_nzc_zu_db, texture_rectangle_16bit_c2_nzc_zu_dn, texture_rectangle_16bit_c2_nzc_zu_dn,
	texture_rectangle_16bit_cc_nzc_zu_dm, texture_rectangle_16bit_cc_nzc_zu_db, texture_rectangle_16bit_cc_nzc_zu_dn, texture_rectangle_16bit_cc_nzc_zu_dn,
	texture_rectangle_16bit_cf_nzc_zu_dm, texture_rectangle_16bit_cf_nzc_zu_db, texture_rectangle_16bit_cf_nzc_zu_dn, texture_rectangle_16bit_cf_nzc_zu_dn,
	texture_rectangle_16bit_c1_zc_zu_dm, texture_rectangle_16bit_c1_zc_zu_db, texture_rectangle_16bit_c1_zc_zu_dn, texture_rectangle_16bit_c1_zc_zu_dn,
	texture_rectangle_16bit_c2_zc_zu_dm, texture_rectangle_16bit_c2_zc_zu_db, texture_rectangle_16bit_c2_zc_zu_dn, texture_rectangle_16bit_c2_zc_zu_dn,
	texture_rectangle_16bit_cc_zc_zu_dm, texture_rectangle_16bit_cc_zc_zu_db, texture_rectangle_16bit_cc_zc_zu_dn, texture_rectangle_16bit_cc_zc_zu_dn,
	texture_rectangle_16bit_cf_zc_zu_dm, texture_rectangle_16bit_cf_zc_zu_db, texture_rectangle_16bit_cf_zc_zu_dn, texture_rectangle_16bit_cf_zc_zu_dn,
};

