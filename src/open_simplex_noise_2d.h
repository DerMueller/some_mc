// header only libary

#ifndef OPEN_IMPLEX_NOISE_2D_H
#define OPEN_IMPLEX_NOISE_2D_H
/*
 * OpenSimplex (Simplectic) Noise in C.
 * Ported by Stephen M. Cameron from Kurt Spencer's java implementation
 * 
 * v1.1 (October 5, 2014)
 * - Added 2D and 4D implementations.
 * - Proper gradient sets for all dimensions, from a
 *   dimensionally-generalizable scheme with an actual
 *   rhyme and reason behind it.
 * - Removed default permutation array in favor of
 *   default seed.
 * - Changed seed-based constructor to be independent
 *   of any particular randomization library, so results
 *   will be the same when ported to other languages.
 */
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#ifndef OSNFLOAT
#define OSNFLOAT double
#endif

#define STRETCH_CONSTANT_2D (-0.211324865405187)    /* (1 / sqrt(2 + 1) - 1 ) / 2; */
#define SQUISH_CONSTANT_2D  (0.366025403784439)     /* (sqrt(2 + 1) -1) / 2; */
#define STRETCH_CONSTANT_3D (-1.0 / 6.0)            /* (1 / sqrt(3 + 1) - 1) / 3; */
#define SQUISH_CONSTANT_3D  (1.0 / 3.0)             /* (sqrt(3+1)-1)/3; */
#define STRETCH_CONSTANT_4D (-0.138196601125011)    /* (1 / sqrt(4 + 1) - 1) / 4; */
#define SQUISH_CONSTANT_4D  (0.309016994374947)     /* (sqrt(4 + 1) - 1) / 4; */
	
#define NORM_CONSTANT_2D (47.0)
#define NORM_CONSTANT_3D (103.0)
#define NORM_CONSTANT_4D (30.0)
	
#define DEFAULT_SEED (0LL)

struct osn_context {
	int16_t *perm;
};


/* 
 * Gradients for 2D. They approximate the directions to the
 * vertices of an octagon from the center.
 */
static const int8_t __simplex_gradients2D[] = {
	 5,  2,    2,  5,
	-5,  2,   -2,  5,
	 5, -2,    2, -5,
	-5, -2,   -2, -5,
};

static OSNFLOAT __simplex_extrapolate2(const struct osn_context *ctx, int xsb, int ysb, OSNFLOAT dx, OSNFLOAT dy)
{
	const int16_t *perm = ctx->perm;
	int index = perm[(perm[xsb & 0xFF] + ysb) & 0xFF] & 0x0E;
	return __simplex_gradients2D[index] * dx
		+ __simplex_gradients2D[index + 1] * dy;
}

static int __simplex_fastFloor(OSNFLOAT x) {
	int xi = (int) x;
	return x < xi ? xi - 1 : xi;
}

static int __simplex_allocate_perm(struct osn_context *ctx, int nperm, int ngrad)
{
	if (ctx->perm)
		free(ctx->perm);
	ctx->perm = (int16_t *) malloc(sizeof(*ctx->perm) * nperm); 
	if (!ctx->perm)
		return -ENOMEM;
	return 0;
}

int open_simplex_noise_init_perm(struct osn_context *ctx, int16_t p[], int nelements)
{
	int rc;

	rc = __simplex_allocate_perm(ctx, nelements, 256);
	if (rc)
		return rc;
	memcpy(ctx->perm, p, sizeof(*ctx->perm) * nelements);
	return 0;
}

/*	
 * Initializes using a permutation array generated from a 64-bit seed.
 * Generates a proper permutation (i.e. doesn't merely perform N successive pair
 * swaps on a base array).  Uses a simple 64-bit LCG.
 */
int open_simplex_noise(struct osn_context **ctx, int64_t seed)
{
	int rc;
	int16_t source[256];
	int i;
	int16_t *perm;
	int r;

	*ctx = (struct osn_context *) malloc(sizeof(**ctx));
	if (!(*ctx))
		return -ENOMEM;
	(*ctx)->perm = NULL;
	
	rc = __simplex_allocate_perm(*ctx, 256, 256);
	if (rc) {
		free(*ctx);
		return rc;
	}

	perm = (*ctx)->perm;
	
	uint64_t seedU = seed;
	for (i = 0; i < 256; i++)
		source[i] = (int16_t) i;
	seedU = seedU * 6364136223846793005ULL + 1442695040888963407ULL;
	seedU = seedU * 6364136223846793005ULL + 1442695040888963407ULL;
	seedU = seedU * 6364136223846793005ULL + 1442695040888963407ULL;
	for (i = 255; i >= 0; i--) {
		seedU = seedU * 6364136223846793005ULL + 1442695040888963407ULL;
		r = (int)((seedU + 31) % (i + 1));
		if (r < 0)
			r += (i + 1);
		perm[i] = source[r];
		source[r] = source[i];
	}
	return 0;
}

void open_simplex_noise_free(struct osn_context *ctx)
{
	if (!ctx)
		return;
	if (ctx->perm) {
		free(ctx->perm);
		ctx->perm = NULL;	
	}
	free(ctx);
}

/* 2D OpenSimplex (Simplectic) Noise. */
OSNFLOAT open_simplex_noise2(const struct osn_context *ctx, OSNFLOAT x, OSNFLOAT y)
{
	
	/* Place input coordinates onto grid. */
	OSNFLOAT stretchOffset = (x + y) * STRETCH_CONSTANT_2D;
	OSNFLOAT xs = x + stretchOffset;
	OSNFLOAT ys = y + stretchOffset;
		
	/* Floor to get grid coordinates of rhombus (stretched square) super-cell origin. */
	int xsb = __simplex_fastFloor(xs);
	int ysb = __simplex_fastFloor(ys);
		
	/* Skew out to get actual coordinates of rhombus origin. We'll need these later. */
	OSNFLOAT squishOffset = (xsb + ysb) * SQUISH_CONSTANT_2D;
	OSNFLOAT xb = xsb + squishOffset;
	OSNFLOAT yb = ysb + squishOffset;
		
	/* Compute grid coordinates relative to rhombus origin. */
	OSNFLOAT xins = xs - xsb;
	OSNFLOAT yins = ys - ysb;
		
	/* Sum those together to get a value that determines which region we're in. */
	OSNFLOAT inSum = xins + yins;

	/* Positions relative to origin point. */
	OSNFLOAT dx0 = x - xb;
	OSNFLOAT dy0 = y - yb;
		
	/* We'll be defining these inside the next block and using them afterwards. */
	OSNFLOAT dx_ext, dy_ext;
	int xsv_ext, ysv_ext;

	OSNFLOAT dx1;
	OSNFLOAT dy1;
	OSNFLOAT attn1;
	OSNFLOAT dx2;
	OSNFLOAT dy2;
	OSNFLOAT attn2;
	OSNFLOAT zins;
	OSNFLOAT attn0;
	OSNFLOAT attn_ext;

	OSNFLOAT value = 0;

	/* Contribution (1,0) */
	dx1 = dx0 - 1 - SQUISH_CONSTANT_2D;
	dy1 = dy0 - 0 - SQUISH_CONSTANT_2D;
	attn1 = 2 - dx1 * dx1 - dy1 * dy1;
	if (attn1 > 0) {
		attn1 *= attn1;
		value += attn1 * attn1 * __simplex_extrapolate2(ctx, xsb + 1, ysb + 0, dx1, dy1);
	}

	/* Contribution (0,1) */
	dx2 = dx0 - 0 - SQUISH_CONSTANT_2D;
	dy2 = dy0 - 1 - SQUISH_CONSTANT_2D;
	attn2 = 2 - dx2 * dx2 - dy2 * dy2;
	if (attn2 > 0) {
		attn2 *= attn2;
		value += attn2 * attn2 * __simplex_extrapolate2(ctx, xsb + 0, ysb + 1, dx2, dy2);
	}
		
	if (inSum <= 1) { /* We're inside the triangle (2-Simplex) at (0,0) */
		zins = 1 - inSum;
		if (zins > xins || zins > yins) { /* (0,0) is one of the closest two triangular vertices */
			if (xins > yins) {
				xsv_ext = xsb + 1;
				ysv_ext = ysb - 1;
				dx_ext = dx0 - 1;
				dy_ext = dy0 + 1;
			} else {
				xsv_ext = xsb - 1;
				ysv_ext = ysb + 1;
				dx_ext = dx0 + 1;
				dy_ext = dy0 - 1;
			}
		} else { /* (1,0) and (0,1) are the closest two vertices. */
			xsv_ext = xsb + 1;
			ysv_ext = ysb + 1;
			dx_ext = dx0 - 1 - 2 * SQUISH_CONSTANT_2D;
			dy_ext = dy0 - 1 - 2 * SQUISH_CONSTANT_2D;
		}
	} else { /* We're inside the triangle (2-Simplex) at (1,1) */
		zins = 2 - inSum;
		if (zins < xins || zins < yins) { /* (0,0) is one of the closest two triangular vertices */
			if (xins > yins) {
				xsv_ext = xsb + 2;
				ysv_ext = ysb + 0;
				dx_ext = dx0 - 2 - 2 * SQUISH_CONSTANT_2D;
				dy_ext = dy0 + 0 - 2 * SQUISH_CONSTANT_2D;
			} else {
				xsv_ext = xsb + 0;
				ysv_ext = ysb + 2;
				dx_ext = dx0 + 0 - 2 * SQUISH_CONSTANT_2D;
				dy_ext = dy0 - 2 - 2 * SQUISH_CONSTANT_2D;
			}
		} else { /* (1,0) and (0,1) are the closest two vertices. */
			dx_ext = dx0;
			dy_ext = dy0;
			xsv_ext = xsb;
			ysv_ext = ysb;
		}
		xsb += 1;
		ysb += 1;
		dx0 = dx0 - 1 - 2 * SQUISH_CONSTANT_2D;
		dy0 = dy0 - 1 - 2 * SQUISH_CONSTANT_2D;
	}
		
	/* Contribution (0,0) or (1,1) */
	attn0 = 2 - dx0 * dx0 - dy0 * dy0;
	if (attn0 > 0) {
		attn0 *= attn0;
		value += attn0 * attn0 * __simplex_extrapolate2(ctx, xsb, ysb, dx0, dy0);
	}
	
	/* Extra Vertex */
	attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext;
	if (attn_ext > 0) {
		attn_ext *= attn_ext;
		value += attn_ext * attn_ext * __simplex_extrapolate2(ctx, xsv_ext, ysv_ext, dx_ext, dy_ext);
	}
	
	return value / NORM_CONSTANT_2D;
}

#endif