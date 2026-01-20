/*
**	Author: zerico2005 (2023 - 2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRACEXP_INTERNAL_H
#define FRACEXP_INTERNAL_H

#include "../PortCE_Common.h"
//#define FracExp_Internal_Debug_Print

#ifdef FracExp_Internal_Debug_Print
	#define printfDebug(...) printf(__VA_ARGS__)
	#define fflushDebug(buf) fflush(buf)
	#define printFlushDebug(buf) printf(__VA_ARGS__); fflush(buf)
#else
	#define printfDebug(...)
	#define fflushDebug(buf)
	#define printFlushDebug(...)
#endif

struct Param_List {
	size_t pos;
	size_t len;
	size_t depth;
	bool isValue;
};

void printText(const char* text, size_t len);

void printParamText(const char* raw, const Param_List* item);

void printParamTextN(const char* raw, const Param_List* item);

size_t clean_frac_raw(char* raw, size_t len);

/*
**	This function takes in the raw file, and converts them into Param_List items
*/
void generate_Param_List(char* raw, size_t len, Param_List** param_list_ptr, size_t* param_len_ptr);
bool strictCompareText(const char* strA, size_t lenA, const char* strB, size_t lenB);
int64_t getNumberFromText(const char* str, size_t len, uint8_t base);

const Param_List* getParameter(const char* raw, const char* path, const Param_List* param_list, size_t param_len);
size_t getParameterArrayLength(const char* raw, const Param_List* item, const Param_List* list, size_t len);

char paramCharAt(const char* raw, const Param_List* item, size_t index);
size_t getTextFromParam(const char* raw, const Param_List* item, char* buf, size_t len);
void copyHex(const char* raw, const Param_List* item, uint64_t* hex, size_t len);

int correctWindowsFileName(char** ret, const char* path);
int fixWindowsFileName(char** ret, const char* path);


#endif /* FRACEXP_INTERNAL_H */
