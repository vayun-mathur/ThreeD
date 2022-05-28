#pragma once
#include <random>
#include <math.h>
#undef max
#undef min

class PerlinNoise {
public:
	PerlinNoise(int seed, int octaves, float amplitude, float roughness)
		: seed(seed), octaves(octaves), amplitude(amplitude), roughness(roughness) {

	}
	PerlinNoise(int octaves, float amplitude, float roughness)
		: PerlinNoise(rand(), octaves, amplitude, roughness) {

	}

	float getAmplitude() {
		return amplitude;
	}

	float getPerlinNoise(vec3 v) {
		return getPerlinNoise(v.x, v.y, v.z);
	}

	float getPerlinNoise(float x, float y, float z) {
		float total = 0;
		float d = (float)pow(2, octaves - 1);
		for (int i = 0; i < octaves; i++) {
			float freq = (float)(pow(2, i) / d);
			float amp = (float)pow(roughness, i) * amplitude;
			total += getInterpolatedNoise(x * freq, y * freq, z * freq) * amp;
		}
		return total;
	}

	float getSmoothNoise(int x, int y, int z) {
		float corners = (getNoise(x - 1, y - 1, z-1) + getNoise(x + 1, y - 1, z - 1) + getNoise(x - 1, y + 1, z - 1)
			+ getNoise(x + 1, y + 1, z - 1) + getNoise(x - 1, y - 1, z + 1) + getNoise(x + 1, y - 1, z + 1) + getNoise(x - 1, y + 1, z + 1)
			+ getNoise(x + 1, y + 1, z + 1)) / 32;
		float sides = (getNoise(x - 1, y, z) + getNoise(x + 1, y, z) + getNoise(x, y - 1, z) + getNoise(x, y + 1, z) + getNoise(x, y, z-1) + getNoise(x, y, z+1)) / 12;
		float center = getNoise(x, y, z) / 4;
		return corners + sides + center;
	}

	float getNoise(vec3 v) {
		return getNoise(v.x, v.y, v.z);
	}

	float getNoise(int x, int y, int z) {
		unsigned long long seed = z * 2347843 + x * 325176 + y * 49632 + this->seed;
		seed = (seed ^ 0x5DEECE66Dull) & ((1ull << 48) - 1);
		seed = (seed * 0x5DEECE66Dull + 0xBull) & ((1ull << 48) - 1);
		float r = ((float)seed) / (1ull << 48);
		return r * 2.f - 1.f;
	}

	float getInterpolatedNoise(float x, float y, float z) {
		int intX = (int)x;
		float fracX = x - intX;
		int intY = (int)y;
		float fracY = y - intY;
		int intZ = (int)z;
		float fracZ = z - intZ;

		float v1 = getSmoothNoise(intX, intY, intZ);
		float v2 = getSmoothNoise(intX + 1, intY, intZ);
		float v3 = getSmoothNoise(intX, intY + 1, intZ);
		float v4 = getSmoothNoise(intX + 1, intY + 1, intZ);
		float v5 = getSmoothNoise(intX, intY, intZ + 1);
		float v6 = getSmoothNoise(intX + 1, intY, intZ + 1);
		float v7 = getSmoothNoise(intX, intY + 1, intZ + 1);
		float v8 = getSmoothNoise(intX + 1, intY + 1, intZ + 1);
		float i1 = interpolate(v1, v2, fracX);
		float i2 = interpolate(v3, v4, fracX);
		float i3 = interpolate(v5, v6, fracX);
		float i4 = interpolate(v7, v8, fracX);
		float f1 = interpolate(i1, i2, fracY);
		float f2 = interpolate(i3, i4, fracY);
		return interpolate(f1, f2, fracZ);
	}

	float interpolate(float a, float b, float blend) {
		double theta = blend * 3.14159;
		float f = (float)((1.f - cosf(theta)) * 0.5f);
		return a * (1 - f) + b * f;
	}

private:
	int seed;
	int octaves;
	float roughness;
	float amplitude;
};
/*
#define clamp(value, minimum, maximum) max(min(value, maximum), minimum);

class ColorGenerator {
public:
	ColorGenerator(vec3* biomeColors, int biomes, float spread)
		: biomeColors(biomeColors), spread(spread), halfSpread(spread / 2), part(1 / (biomes - 1.f)) {
	}

	vec3** generateColours(float** heights, float gridSize, float amplitude) {
		vec3** colors = new vec3 * [gridSize + 1];
		for (int z = 0; z <= gridSize; z++) {
			colors[z] = new vec3[gridSize + 1];
			for (int x = 0; x <= gridSize; x++) {
				colors[z][x] = calculateColour(heights[z][x], amplitude);
			}
		}
		return colors;
	}

	vec3 calculateColour(float height, float amplitude) {
		float value = (height + amplitude) / (amplitude * 2);
		value = clamp((value - halfSpread) * (1.f / spread), 0.f, 0.9999f);
		int firstBiome = (int)floor(value / part);
		float blend = (value - (firstBiome * part)) / part;
		return vec3::lerp(biomeColors[firstBiome], biomeColors[firstBiome + 1], blend);
	}
private:
	float spread;
	float halfSpread;

	vec3* biomeColors;
	float part;
};
*/
inline vec3 floor(vec3 x) {
    return vec3(floor(x.x), floor(x.y), floor(x.z));
}

inline vec4 floor(vec4 x) {
    return vec4(floor(x.x), floor(x.y), floor(x.z), floor(x.w));
}

inline vec4 max(vec4 v1, vec4 v2) {
	return vec4(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z), std::max(v1.w, v2.w));
}

inline vec3 mod289(vec3 x)
{
    return x - floor(x / 289.0) * 289.0;
}

inline vec4 mod289(vec4 x)
{
    return x - floor(x / 289.0) * 289.0;
}

inline vec4 abs(vec4 v) {
	return vec4(abs(v.x), abs(v.y), abs(v.z), abs(v.w));
}

inline vec4 permute(vec4 x)
{
    return mod289((x * 34.0 + 1.0) * x);
}

inline vec4 taylorInvSqrt(vec4 r)
{
    return vec4(1.79284291400159) - r * 0.85373472095314;
}

inline float snoise(vec3 v)
{
    const vec2 C = vec2(1.0 / 6.0, 1.0 / 3.0);

    // First corner
    vec3 i = floor(v + vec3::dot(v, C.y));
    vec3 x0 = v - i + vec3::dot(i, C.x);

    // Other corners
	vec3 x02 = vec3(x0.y, x0.z, x0.x);
	vec3 g = vec3(x02.x >= x0.x, x02.y >= x0.y, x02.z >= x0.z);
    vec3 l = vec3(1.0) - g;
	vec3 l2 = vec3(l.z, l.x, l.y);
    vec3 i1 = min(g, l2);
    vec3 i2 = max(g, l2);

    // x1 = x0 - i1  + 1.0 * C.xxx;
    // x2 = x0 - i2  + 2.0 * C.xxx;
    // x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec3 x1 = x0 - i1 + C.x;
    vec3 x2 = x0 - i2 + C.y;
    vec3 x3 = x0 - 0.5;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec4 p =
        permute(permute(permute(vec4(i.z) + vec4(0.0, i1.z, i2.z, 1.0))
            + i.y + vec4(0.0, i1.y, i2.y, 1.0))
            + i.x + vec4(0.0, i1.x, i2.x, 1.0));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    vec4 j = p - vec4(49.0) * floor(p / 49.0);  // mod(p,7*7)

    vec4 x_ = floor(j / 7.0);
    vec4 y_ = floor(j - vec4(7.0) * x_);  // mod(j,N)

    vec4 x = (x_ * 2.0 + 0.5) / 7.0 - 1.0;
    vec4 y = (y_ * 2.0 + 0.5) / 7.0 - 1.0;

    vec4 h = vec4(1.0) - abs(x) - abs(y);

    vec4 b0 = vec4(x.xy(), y.xy());
    vec4 b1 = vec4(x.zw(), y.zw());

    //vec4 s0 = vec4(lessThan(b0, 0.0)) * 2.0 - 1.0;
    //vec4 s1 = vec4(lessThan(b1, 0.0)) * 2.0 - 1.0;
    vec4 s0 = floor(b0) * 2.0 + 1.0;
    vec4 s1 = floor(b1) * 2.0 + 1.0;
	vec4 sh = vec4(h.x > 0, h.y > 0, h.z > 0, h.w > 0) * -1;
	vec4 a0 = vec4(b0.x + s0.x + sh.x, b0.z + s0.z + sh.x, b0.y + s0.y + sh.y, b0.w + s0.w + sh.y);
	vec4 a1 = vec4(b0.x + s0.x + sh.z, b0.z + s0.z + sh.z, b0.y + s0.y + sh.w, b0.w + s0.w + sh.w);

    vec3 g0 = vec3(a0.xy(), h.x);
    vec3 g1 = vec3(a0.zw(), h.y);
    vec3 g2 = vec3(a1.xy(), h.z);
    vec3 g3 = vec3(a1.zw(), h.w);

    // Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(vec3::dot(g0, g0), vec3::dot(g1, g1), vec3::dot(g2, g2), vec3::dot(g3, g3)));
    g0 *= norm.x;
    g1 *= norm.y;
    g2 *= norm.z;
    g3 *= norm.w;

    // Mix final noise value
    vec4 m = max(vec4(0.6) - vec4(vec3::dot(x0, x0), vec3::dot(x1, x1), vec3::dot(x2, x2), vec3::dot(x3, x3)), 0.0);
    m = m * m;
    m = m * m;

    vec4 px = vec4(vec3::dot(x0, g0), vec3::dot(x1, g1), vec3::dot(x2, g2), vec3::dot(x3, g3));
    return 42.0 * vec4::dot(m, px);
}