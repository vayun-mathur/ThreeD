#pragma once
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>

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

	float getPerlinNoise(int x, int y) {
		float total = 0;
		float d = (float)pow(2, octaves - 1);
		for (int i = 0; i < octaves; i++) {
			float freq = (float)(pow(2, i) / d);
			float amp = (float)pow(roughness, i) * amplitude;
			total += getInterpolatedNoise(x * freq, y * freq) * amp;
		}
		return total;
	}

	float getSmoothNoise(int x, int y) {
		float corners = (getNoise(x - 1, y - 1) + getNoise(x + 1, y - 1) + getNoise(x - 1, y + 1)
			+ getNoise(x + 1, y + 1)) / 16;
		float sides = (getNoise(x - 1, y) + getNoise(x + 1, y) + getNoise(x, y - 1) + getNoise(x, y + 1)) / 8;
		float center = getNoise(x, y) / 4;
		return corners + sides + center;
	}

	float getNoise(int x, int y) {
		unsigned long long seed = x * 49632 + y * 325176 + this->seed;
		seed = (seed ^ 0x5DEECE66Dull) & ((1ull << 48) - 1);
		seed = (seed * 0x5DEECE66Dull + 0xBull) & ((1ull << 48) - 1);
		float r = ((float)seed)/(1ull<<48);
		return r * 2.f - 1.f;
	}

	float getInterpolatedNoise(float x, float y) {
		int intX = (int)x;
		float fracX = x - intX;
		int intY = (int)y;
		float fracY = y - intY;

		float v1 = getSmoothNoise(intX, intY);
		float v2 = getSmoothNoise(intX + 1, intY);
		float v3 = getSmoothNoise(intX, intY + 1);
		float v4 = getSmoothNoise(intX + 1, intY + 1);
		float i1 = interpolate(v1, v2, fracX);
		float i2 = interpolate(v3, v4, fracX);
		return interpolate(i1, i2, fracY);
	}

	float interpolate(float a, float b, float blend) {
		double theta = blend * M_PI;
		float f = (float)((1.f - cosf(theta)) * 0.5f);
		return a * (1 - f) + b * f;
	}

private:
	int seed;
	int octaves;
	float roughness;
	float amplitude;
};

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