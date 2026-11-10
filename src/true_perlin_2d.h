#ifndef TRUE_PERLIN_2D_H
#define TRUE_PERLIN_2D_H

#include <math.h>
#include <stdint.h>

static unsigned char __perlin2d_permutation[256] = {
	151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,
	136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
	55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
	159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
	47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,
	39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
	81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,
	205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static int perlin2d_repeat = 0;

// from 64 bit seed, generate a permutation array
void Perlin2DSetSeed(int64_t seed) {
	uint64_t seedU = seed;
	
	for (int i = 0; i < 256; i++) {
		__perlin2d_permutation[i] = (uint8_t) i;
	}
	seedU = seedU * 2862933555777941757ULL + 3037000493ULL;
	for (int i = 255; i >= 0; i--) {
		seedU = seedU * 2862933555777941757ULL + 3037000493ULL;
		int r = (int)((seedU + 31) % (i + 1));
		if (r < 0) r += (i + 1);
		uint8_t swap = __perlin2d_permutation[i];
		__perlin2d_permutation[i] = __perlin2d_permutation[r];
		__perlin2d_permutation[r] = swap;
	}

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

static inline int perlin2d_hash(int x, int y)
{
	int index = (__perlin2d_permutation[x & 255] + (y & 255)) & 255;
	return __perlin2d_permutation[index];
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

static inline double Perlin2D(double x, double y)
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
		perlin2d_gradient(perlin2d_hash(xi, yi), xf, yf),
		perlin2d_gradient(perlin2d_hash(xi1, yi), xf - 1.0, yf),
		u);
	double upper = perlin2d_lerp(
		perlin2d_gradient(perlin2d_hash(xi, yi1), xf, yf - 1.0),
		perlin2d_gradient(perlin2d_hash(xi1, yi1), xf - 1.0, yf - 1.0),
		u);

	return perlin2d_lerp(lower, upper, v) * 0.5 + 0.5;
}

static inline double OctavePerlin2D(double x, double y, int octaves, double persistence)
{
	double total = 0.0;
	double frequency = 1.0;
	double amplitude = 1.0;
	double max_value = 0.0;

	for (int i = 0; i < octaves; i++) {
		total += Perlin2D(x * frequency, y * frequency) * amplitude;
		max_value += amplitude;
		amplitude *= persistence;
		frequency *= 2.0;
	}

	return max_value > 0.0 ? total / max_value : 0.0;
}

#endif
