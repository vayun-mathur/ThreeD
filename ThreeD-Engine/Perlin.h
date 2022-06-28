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