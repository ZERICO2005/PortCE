#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>
#include <algorithm>

	// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
	template <typename T, typename fpX>
	void getRGBfromHSV(T* r, T* g, T* b, fpX hue, fpX sat, fpX val) {
		uint8_t hueIndex = (uint8_t)((fpX)hue / (fpX)60.0) % 6;
		fpX f = ((fpX)hue / (fpX)60.0) - (fpX)floor((fpX)hue / (fpX)60.0);
		val *= (fpX)255.0;
		T vR = (T)val;
		T pR = (T)(val * ((fpX)1.0 - sat));
		T qR = (T)(val * ((fpX)1.0 - f * sat));
		T tR = (T)(val * ((fpX)1.0 - ((fpX)1.0 - f) * sat));
		T R = 0; T G = 0; T B = 0;
		switch(hueIndex) {
			case 0:
			R = vR; G = tR; B = pR;
			break;
			case 1:
			R = qR; G = vR; B = pR;
			break;
			case 2:
			R = pR; G = vR; B = tR;
			break;
			case 3:
			R = pR; G = qR; B = vR;
			break;
			case 4:
			R = tR; G = pR; B = vR;
			break;
			case 5:
			R = vR; G = pR; B = qR;
			break;
		}
		if (r != nullptr) { *r = R; }
		if (g != nullptr) { *g = G; }
		if (b != nullptr) { *b = B; }
	}

	// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
	uint32_t getRGBfromHSV(double hue, double sat, double val);
	// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
	uint32_t getRGBfromHSV(float hue, float sat, float val);

	// (&R,&G,&B,&A) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
	void getRGBAfromHSVA(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, double hue, double sat, double val, double alpha);
	// (&R,&G,&B,&A) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
	void getRGBAfromHSVA(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, float hue, float sat, float val, float alpha);
	// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
	uint32_t getRGBAfromHSVA(double hue, double sat, double val, double alpha);
	// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
	uint32_t getRGBAfromHSVA(float hue, float sat, float val, float alpha);


// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
inline void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, double hue, double sat, double val) {
	uint8_t hi = (uint8_t)(hue / 60.0) % 6;
	double f = (hue / 60.0) - floor(hue / 60.0);
	val *= 255.0;
	uint8_t vR = (uint8_t)val;
	uint8_t pR = (uint8_t)(val * (1.0 - sat));
	uint8_t qR = (uint8_t)(val * (1.0 - f * sat));
	uint8_t tR = (uint8_t)(val * (1.0 - (1.0 - f) * sat));
	uint8_t R = 0; uint8_t G = 0; uint8_t B = 0;
	switch(hi) {
		case 0:
		R = vR; G = tR; B = pR;
		break;
		case 1:
		R = qR; G = vR; B = pR;
		break;
		case 2:
		R = pR; G = vR; B = tR;
		break;
		case 3:
		R = pR; G = qR; B = vR;
		break;
		case 4:
		R = tR; G = pR; B = vR;
		break;
		case 5:
		R = vR; G = pR; B = qR;
		break;
	}
	if (r != nullptr) { *r = R; }
	if (g != nullptr) { *g = G; }
	if (b != nullptr) { *b = B; }
}

// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
inline void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, float hue, float sat, float val) {
	uint8_t hi = (uint8_t)(hue / 60.0f) % 6;
	float f = (hue / 60.0f) - (float)floor(hue / 60.0f);
	val *= 255.0f;
	uint8_t vR = (uint8_t)val;
	uint8_t pR = (uint8_t)(val * (1.0f - sat));
	uint8_t qR = (uint8_t)(val * (1.0f - f * sat));
	uint8_t tR = (uint8_t)(val * (1.0f - (1.0f - f) * sat));
	uint8_t R = 0; uint8_t G = 0; uint8_t B = 0;
	switch(hi) {
		case 0:
		R = vR; G = tR; B = pR;
		break;
		case 1:
		R = qR; G = vR; B = pR;
		break;
		case 2:
		R = pR; G = vR; B = tR;
		break;
		case 3:
		R = pR; G = qR; B = vR;
		break;
		case 4:
		R = tR; G = pR; B = vR;
		break;
		case 5:
		R = vR; G = pR; B = qR;
		break;
	}
	if (r != nullptr) { *r = R; }
	if (g != nullptr) { *g = G; }
	if (b != nullptr) { *b = B; }
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
inline uint32_t getRGBfromHSV(double hue, double sat, double val) {
	uint8_t r, g, b;
	getRGBfromHSV(&r, &g, &b, hue, sat, val);
	return ((uint32_t)r << 16) + ((uint32_t)g << 8) + (uint32_t)b;
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
inline uint32_t getRGBfromHSV(float hue, float sat, float val) {
	uint8_t r, g, b;
	getRGBfromHSV(&r, &g, &b, hue, sat, val);
	return ((uint32_t)r << 16) + ((uint32_t)g << 8) + (uint32_t)b;
}

/* Color 32bit */

// (&R,&G,&B,&A) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
inline void getRGBAfromHSVA(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, double hue, double sat, double val, double alpha) {
	if (a != nullptr) {
		alpha = std::clamp(alpha, 0.0, 1.0);
		*a = (uint8_t)(alpha * 255.0);
	}
	getRGBfromHSV(r,g,b,hue,sat,val);
}

// (&R,&G,&B,&A) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
inline void getRGBAfromHSVA(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, float hue, float sat, float val, float alpha) {
	if (a != nullptr) {
		alpha = std::clamp(alpha, 0.0f, 1.0f);
		*a = (uint8_t)(alpha * 255.0f);
	}
	getRGBfromHSV(r,g,b,hue,sat,val);
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
inline uint32_t getRGBAfromHSVA(double hue, double sat, double val, double alpha) {
	uint8_t r, g, b, a;
	getRGBAfromHSVA(&r, &g, &b, &a, hue, sat, val, alpha);
	return ((uint32_t)r << 16) + ((uint32_t)g << 8) + (uint32_t)b;
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
inline uint32_t getRGBAfromHSVA(float hue, float sat, float val, float alpha) {
	uint8_t r, g, b, a;
	getRGBAfromHSVA(&r, &g, &b, &a, hue, sat, val, alpha);
	return ((uint32_t)r << 16) + ((uint32_t)g << 8) + (uint32_t)b;
}

#endif /* COLOR_HPP */
