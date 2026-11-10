#ifndef TRUE_PERLIN_2D_H
#define TRUE_PERLIN_2D_H

#include <math.h>
#include <stdint.h>

struct Perlin2DContext {
	int16_t *perm;
};

static int __perlin2d_allocate_perm(struct Perlin2DContext *ctx, int nperm) {
	// int16_t *newperm = (int16_t *) malloc(sizeof(*newperm) * nperm);
    // if (!newperm)
    //     return -ENOMEM;
    // free(ctx->perm);        // free old only after new succeeds
    // ctx->perm = newperm;

	if (ctx->perm)
		free(ctx->perm);
	ctx->perm = (int16_t *) malloc(sizeof(*ctx->perm) * nperm); 
	if (!ctx->perm)
		return -ENOMEM;

    return 0;
}

static int perlin2d_repeat = 0;

// from 64 bit seed, generate a permutation array
int Perlin2DSetSeed(struct Perlin2DContext **ctx, int64_t seed) {
	if (!ctx) return -EINVAL;
    if (!*ctx) {
        *ctx = (struct Perlin2DContext *) calloc(1, sizeof(**ctx));
        if (!*ctx) return -ENOMEM;
    }
	uint64_t seedU = seed;

	int16_t source[256];

	if (!*ctx) {
		*ctx = (struct Perlin2DContext *) malloc(sizeof(**ctx));
		if (!*ctx) return -ENOMEM;
		(*ctx)->perm = NULL;  // <--- Explicitly safe
	}
	int rc = __perlin2d_allocate_perm(*ctx, 256);
    if (rc) return rc;

	int16_t *perm;
	perm = (*ctx)->perm;
	
	for (int i = 0; i < 256; i++) {
		source[i] = (int16_t) i;
	}
	seedU = seedU * 2862933555777941757ULL + 3037000493ULL;
	for (int i = 255; i >= 0; i--) {
		seedU = seedU * 2862933555777941757ULL + 3037000493ULL;
		int r = (int)((seedU + 31) % (i + 1));
		if (r < 0) r += (i + 1);
		perm[i] = source[r];
		source[r] = source[i];
	}
	return 0;
}

static inline void Perlin2DSetRepeat(int repeat)
{
	perlin2d_repeat = repeat > 0 ? repeat : 0;
}

static inline int perlin2d_wrap_cell(int cell)
{
	if (perlin2d_repeat > 0) {
		cell %= perlin2d_repeat;
		if (cell < 0) cell += perlin2d_repeat;
		return cell & 255;
	}

	return cell & 255;
}

static inline int perlin2d_hash(struct Perlin2DContext *ctx, int x, int y)
{
	const int16_t *perm = ctx->perm;
	int index = (perm[x & 255] + (y & 255)) & 255;
	return perm[index];
}

static inline double perlin2d_fade(double t)
{
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

static inline double perlin2d_lerp(double a, double b, double t)
{
	return a + t * (b - a);
}

static inline double perlin2d_gradient(int hash, double x, double y)
{
	static const double diagonal = 0.70710678118654752440;

	switch (hash & 7) {
	case 0: return x;
	case 1: return -x;
	case 2: return y;
	case 3: return -y;
	case 4: return (x + y) * diagonal;
	case 5: return (x - y) * diagonal;
	case 6: return (-x + y) * diagonal;
	default: return (-x - y) * diagonal;
	}
}

static inline double Perlin2D(struct Perlin2DContext *ctx, double x, double y)
{
	if (perlin2d_repeat > 0) {
		x = fmod(x, perlin2d_repeat);
		y = fmod(y, perlin2d_repeat);
		if (x < 0.0) x += perlin2d_repeat;
		if (y < 0.0) y += perlin2d_repeat;
	}

	double floor_x = floor(x);
	double floor_y = floor(y);
	int xi = perlin2d_wrap_cell((int)floor_x);
	int yi = perlin2d_wrap_cell((int)floor_y);
	int xi1 = perlin2d_wrap_cell((int)floor_x + 1);
	int yi1 = perlin2d_wrap_cell((int)floor_y + 1);
	double xf = x - floor_x;
	double yf = y - floor_y;
	double u = perlin2d_fade(xf);
	double v = perlin2d_fade(yf);

	double lower = perlin2d_lerp(
		perlin2d_gradient(perlin2d_hash(ctx, xi, yi), xf, yf),
		perlin2d_gradient(perlin2d_hash(ctx, xi1, yi), xf - 1.0, yf),
		u);
	double upper = perlin2d_lerp(
		perlin2d_gradient(perlin2d_hash(ctx, xi, yi1), xf, yf - 1.0),
		perlin2d_gradient(perlin2d_hash(ctx, xi1, yi1), xf - 1.0, yf - 1.0),
		u);

	return perlin2d_lerp(lower, upper, v) * 0.5 + 0.5;
}

static inline double OctavePerlin2D(struct Perlin2DContext *ctx, double x, double y, int octaves, double persistence)
{
	double total = 0.0;
	double frequency = 1.0;
	double amplitude = 1.0;
	double max_value = 0.0;

	for (int i = 0; i < octaves; i++) {
		total += Perlin2D(ctx, x * frequency, y * frequency) * amplitude;
		max_value += amplitude;
		amplitude *= persistence;
		frequency *= 2.0;
	}

	return max_value > 0.0 ? total / max_value : 0.0;
}

#endif
