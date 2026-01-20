/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef COPYBUFFER_H
#define COPYBUFFER_H

#include <cstdint>
#include <cstring>

struct BufferBox {
	uint8_t* vram;
	int resX;
	int resY;
	size_t channels;
	size_t padding;
};

void initBufferBox(BufferBox* box, uint8_t* buf, int resX, int resY, size_t channels, size_t padding = 0);

size_t getBufferBoxPitch(const BufferBox* box);

size_t getBufferBoxSize(const BufferBox* box);

// Returns false in BufferBox should not be written to
bool validateBufferBox(const BufferBox* box, bool checkForNullVram = true);
// Prints Errors and returns false in BufferBox should not be written to
bool printValidateBufferBox(const BufferBox* box);

/* buf{src, dst}, src{cord, size}, dst{cord, size}, allowClipping | Copies a portion of Src buffer to a portion of Dst */
void copyBuffer(
	BufferBox bufSrc, BufferBox bufDst,
	int32_t sx0, int32_t sy0, int sx1, int sy1,
	int32_t dx0, int32_t dy0, int dx1, int dy1,
	bool allowClipping
);

/* buf{src, dst}, dst{cord} allowClipping | Copies Src buffer to a location on Dst */
void copyBuffer(BufferBox bufSrc, BufferBox bufDst, int32_t x, int32_t y, bool allowClipping);

/* buf{src, dst}, allowClipping | Copies Src buffer to Dst */
void copyBuffer(BufferBox bufSrc, BufferBox bufDst, bool allowClipping);

// Copies all of src into dst + verticalOffset if the pitch of each BufferBox is the same
inline void copyBuffer_VeritcalOffset(
	const BufferBox& dst, const BufferBox& src,
	size_t verticalOffset
) {
	if (
		(validateBufferBox(&dst,true) == false) ||
		(validateBufferBox(&src,true) == false) ||
		(verticalOffset >= (size_t)dst.resY)
	) {
		return;
	}
	size_t srcPitch = (size_t)src.resX * src.channels;
	srcPitch += (src.padding != 0 && srcPitch % src.padding != 0) ? (src.padding - (srcPitch % src.padding)) : 0;
	size_t dstPitch = (size_t)dst.resX * dst.channels;
	dstPitch += (dst.padding != 0 && dstPitch % dst.padding != 0) ? (dst.padding - (dstPitch % dst.padding)) : 0;
	if (srcPitch != dstPitch) { return; }

	size_t offset = (verticalOffset) * dstPitch;
	size_t copySize = ((size_t)dst.resY - verticalOffset) * dstPitch;
	if (copySize > (size_t)src.resY * srcPitch) {
		copySize = (size_t)src.resY * srcPitch;
	}
	memcpy(&dst.vram[offset], src.vram, copySize);
}

/**
 * @brief Fills a buffer with a repeating pattern of N bytes
 */
inline void* patternMemcpy(void* __restrict buf, size_t bufSize, const void* __restrict PatternData, size_t PatternSize) {
	if (buf == nullptr || PatternData == nullptr || PatternSize == 0) { return nullptr; }
	if (bufSize == 0) { return buf; } // 0 Bytes to copy
	// if (PatternSize == 1) {
	// 	memset(buf, ((uint8_t*)PatternData)[0], bufSize);
	// 	return buf;
	// }
	if (bufSize <= PatternSize) {
		memcpy(buf, PatternData, bufSize);
		return buf;
	}
	memcpy(buf, PatternData, PatternSize); // Initial Copy
	size_t len = PatternSize;
	size_t pos = PatternSize;

	while (pos + len <= bufSize) {
		memcpy((uint8_t*)buf + pos, buf, len);
		pos += len;
		len *= 2; // Doubles copy size each iteration
	}
	memcpy((uint8_t*)buf + pos, buf, bufSize - len); // Copies the remaining portion
	return buf;
}


/**
 * @brief Assumes the pattern is set in the first N bytes in buf
 */
inline void* inPlacePatternMemcpy(void* __restrict buf, size_t bufSize, size_t PatternSize) {
	if (buf == nullptr || PatternSize == 0) { return nullptr; }
	if (bufSize <= PatternSize) { return buf; }
	// if (PatternSize == 1) {
	// 	memset(buf, ((uint8_t*)buf)[0], bufSize);
	// 	return buf;
	// }
	size_t len = PatternSize;
	size_t pos = PatternSize;
	while (pos + len <= bufSize) {
		memcpy((uint8_t*)buf + pos, buf, len);
		pos += len;
		len *= 2; // Doubles copy size each iteration
	}
	memcpy((uint8_t*)buf + pos, buf, bufSize - len); // Copies the remaining portion
	return 0;
}


#endif /* COPYBUFFER_H */
