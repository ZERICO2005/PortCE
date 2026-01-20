/*
**	Author: zerico2005 (2023 - 2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "fracExp_Internal.h"

void printText(const char* text, size_t len) {
	if (text == NULL || len == 0) {
		printfDebug("\n");
		fflushDebug(stdout);
		return;
	}
	if (text[0] != '\n') {
		printfDebug("\n");
	}
	for (size_t i = 0; i < len; i++) {
		printfDebug("%c",text[i]);
	}
	printfDebug("\n");
	fflushDebug(stdout);
}

// Param_List* param_list; size_t param_len = 0;
// size_t param_start;  size_t param_end;


void printParamText(__attribute__((unused)) const char* raw, const Param_List* item) {
	for (size_t p = 0; p < item->len; p++) { printfDebug("%c",raw[p + item->pos]); }
	fflushDebug(stdout);
}

void printParamTextN(__attribute__((unused)) const char* raw, const Param_List* item) {
	printfDebug("\n");
	for (size_t p = 0; p < item->len; p++) { printfDebug("%c",raw[p + item->pos]); }
	fflushDebug(stdout);
}


size_t clean_frac_raw(char* raw, size_t len) { // Removes comments and blank lines and converts to Unix line endings
	#define flushExtraCharacters(posW); \
		len = posW;\
		for (size_t w = posW; posW < len; posW++) {\
			raw[w] = '\0';\
		}
 	// Converts to Unix line endings
	char prev = '\n';
	size_t posW = 0;
	for (size_t posR = 0; posR < len; posR++) {
		if (raw[posR] == '\r') {
			//#define SupportCarriageReturn
			#ifdef SupportCarriageReturn
			if(frac_raw[posR + 1] != '\n') {
				frac_raw[posW] = frac_raw[posR];
				posW++;
			}
			#endif
		} else {
			raw[posW] = raw[posR];
			posW++;
		}
		prev = raw[posR];
	}
	flushExtraCharacters(posW);
	// Skips comments
	prev = '\n';
	posW = 0;
	bool insideQuotes = false;
	printfDebug("%s",bool_Text(insideQuotes));
	for (size_t posR = 0; posR < len; posR++) {
		if ((raw[posR] == '#' && prev != '\\')) {
			while (posR < len) {
				if (raw[posR] == '\n') {
					posR--;
					break;
				}
				posR++;
			}
		} else {
			raw[posW] = raw[posR];
			posW++;
			if (insideQuotes == true) {

			}
			/*
			if ((frac_raw[posR] == ':' && prev != '\\')) {
				while (posR < len) {
					if (frac_raw[posR] == '\n') {
						posR--;
						insideQuotes = false;
						break;
					}
					posR++;
				}
			}
			*/
		}
		prev = raw[posR];
	}
	flushExtraCharacters(posW);
	//Removes blank lines
	prev = '\n';
	posW = 0;
	for (size_t posR = 0; posR < len; posR++) {
		if (!(raw[posR] == '\n' && prev == '\n')) { // Skips blank lines
			raw[posW] = raw[posR];
			posW++;
		}
		prev = raw[posR];
	}
	flushExtraCharacters(posW);
	return posW;
}

void generate_Param_List(char* raw, size_t len, Param_List** param_list_ptr, size_t* param_len_ptr) {
	if (param_len_ptr == NULL || param_list_ptr == NULL) {
		fprintf(stderr, "\nparam_len_ptr and/or param_list_ptr are/is NULL");
		return;
	}
	Param_List* param_list;
	size_t param_len;
	if (raw == NULL) {
		fprintf(stderr, "Error: frac_raw is NULL");
		return;
	}
	len = clean_frac_raw(raw,len);
	printText(raw,len);
	// Count the total amount of rows, skipping blank lines
	size_t rowCount = 1;
	char prev = '\0';
	for (size_t pos = 0; pos < len; pos++) {
		if (raw[pos] == '\n' && prev != '\n') {
			rowCount++;
		}
		prev = raw[pos];
	}
	printFlushDebug("\nTotal Rows: %" PRIu64,rowCount);
	param_len = rowCount;
	param_list = (Param_List*)calloc(param_len, sizeof(Param_List));

	// Write data to param_list
	prev = '\n';
	size_t pos = 0;
	for (size_t row = 0; row < param_len; row++) {
		bool insideQuotes = false;
		param_list[row].depth = 0;
		param_list[row].len = 0;
		param_list[row].isValue = true;
		// Depth
		while (pos < len && raw[pos] == '\t') {
			param_list[row].depth++;
			prev = raw[pos];
			pos++;
		}
		// Starting pos
		param_list[row].pos = pos;
		// Text length
		while (pos < len) {
			if ((raw[pos] == ':' && insideQuotes == false) || (raw[pos] == '\n')) {
				break;
			}
			if (raw[pos] == '\"' && prev != '\\') {
				if (insideQuotes == false) { insideQuotes = true; } else { insideQuotes = false; }
			}
			prev = raw[pos];
			pos++;
		}
		param_list[row].len = pos - param_list[row].pos;
		if (raw[pos] == ':') {
			param_list[row].isValue = false;
			while (pos < len && raw[pos] != '\n') {
				prev = raw[pos];
				pos++;
			}
		}
		pos++;
		printfDebug("\nrow: %3llu pos: %4llu len: %2llu depth: %1llu %6s\n", row, param_list[row].pos, param_list[row].len, param_list[row].depth, param_list[row].isValue ? "value" : "header");
		for (size_t p = 0; p < param_list[row].len; p++) { printfDebug("%c",raw[p + param_list[row].pos]); }
	}
	*param_len_ptr = param_len;
	*param_list_ptr = param_list;
}

bool strictCompareText(const char* strA, size_t lenA, const char* strB, size_t lenB) {
	// printf("\nstrA: ");
	// for (size_t p = 0; p < lenA; p++) { printf("%c",strA[p]); }
	// printf(" strB: ");
	// for (size_t p = 0; p < lenB; p++) { printf("%c",strB[p]); }
	// fflush(stdout);
	if (lenA != lenB) {
		return false;
	}
	for (size_t i = 0; i < lenA; i++) {
		if (strA[i] == '\0' && strB[i] == '\0') {
			return true;
		}
		if (strA[i] != strB[i]) {
			return false;
		}
	}
	return true;
}

int64_t getNumberFromText(const char* str, size_t len, uint8_t base) {
	if (base < 2 || base > 36) { return 0; }
	size_t pos = 0;
	while (pos < len && str[pos] == ' ') { // Skips whitespace
		pos++;
	}
	bool sign = (str[pos] == '-') ? true : false;
	int64_t acc = 0;
	while (pos < len && str[pos] != '\0') {
		char c = str[pos];
		uint8_t val = UINT8_MAX;
		if (c >= '0' && c <= '9') {
			val = (uint8_t)(c - '0');
		} else if (c >= 'A' && c <= 'Z') {
			val = (uint8_t)(c - 'A' + 10);
		} else if (c >= 'a' && c <= 'z') {
			val = (uint8_t)(c - 'a' + 10);
		}
		if (val < base) {
			acc *= base;
			acc += val;
		} else {
			break;
		}
		pos++;
	}
	return (sign == true) ? -acc : acc;
}

char paramCharAt(const char* raw, const Param_List* item, size_t index) { // Returns '\0' for out of range values
	if (index >= item->len) {
		return '\0';
	}
	return raw[item->pos + index];
}

// Returns size of text
size_t getTextFromParam(const char* raw, const Param_List* item, char* buf, size_t len) {
	if (raw == NULL) {
		return 0;
	}
	if (item == NULL || buf == NULL || len == 0) {
		return 0;
	}
	if (len == 1 || item->len == 0 || strictCompareText(&raw[item->pos], item->len,(char*)"NULL",4)) {
		buf[0] = '\0';
		return 1;
	}
	if (len == 2) {
		buf[0] = paramCharAt(raw,item,0);
		buf[1] = '\0';
		return 2;
	}
	//if (paramCharAt(raw,item,0) )
	if (item->len == 1) {
		buf[0] = paramCharAt(raw,item,0);
	}

	for (size_t i = 0; i < len && i < item->len; i++) {
		buf[i] = raw[item->pos + i];
	}
	return (len < item->len) ? len : item->len;
}

const Param_List* getParameter(const char* raw, const char* path, const Param_List* param_list, size_t param_len) { /* For exporting a singular value */
	if (param_len == 0 || param_list == NULL) {
		fprintf(stderr, "\nparam_len and/or param_list are/is 0/NULL");
		return NULL;
	}
	if (raw == NULL) { return NULL; }
	char prev = '\0';

	size_t i = 0;
	size_t maxDepth = 0;
	while (path[i] != '\0') {
		if (path[i] == '/' && prev != '\'') {
			maxDepth++;
		}
		prev = path[i];
		i++;
	}

	size_t pos0 = 0;
	size_t pos1 = 0;
	size_t select = 0;
	// printFlush("\n");
	for (size_t depth = 0; depth <= maxDepth; depth++) {
		while ((path[pos1] != '\0') && !(path[pos1] == '/' && prev != '\'')) {
			prev = path[pos1];
			pos1++;
		}
		// printf("\nDepth: %" PRIu64 " Len: %" PRIu64 " Searching: ",depth,pos1 - pos0);
		// for (size_t p = pos0; p < pos1; p++) { printf("%c",path[p]); }
		// fflush(stdout);
		size_t row = select;
		// printFlush("\n%" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64,row,param_list[row].depth,depth,param_len);
		for (; row < param_len; row++) {
			if (param_list[row].depth == depth) {
				if (strictCompareText(&path[pos0], pos1 - pos0, &raw[param_list[row].pos], param_list[row].len) == true) {
					if (depth == maxDepth) {
						printFlushDebug("\nExporting %s\n\t",path);
						printParamText(raw,&param_list[row + 1]);
						return &param_list[row + 1];
					}
					select = row + 1;
					// printFlush("\nMatched row %" PRIu64,row);
					break;
				}
			} else if (param_list[row].depth < depth) {
				printFlushDebug("\nDepth Error: Depth %" PRIu64 ", Row %" PRIu64 ", Row-Depth %" PRIu64 ", unable to find %s",depth,row,param_list[row].depth,path);
				return NULL;
			}
		}
		if (row == param_len) {
			printFlushDebug("\nExhaust Error: Depth %" PRIu64 ", unable to find %s",depth,path);
			return NULL;
		}
		pos1++;
		pos0 = pos1;
	}
	// while ((path[pos1] != '\0') && !(path[pos1] == '/' && prev != '\'')) {
	// 	prev = path[pos1];
	// 	pos1++;
	// }
	// printf("\nSearching for value: ");
	// for (size_t p = pos0; p < pos1; p++) { printf("%c",path[p]); }
	// fflush(stdout);
	// for (size_t row = select; row < param_len; row++) {
	// 	if (strictCompareText(&path[pos0], pos1 - pos0, &frac_raw[param_list[row].pos], param_list[row].len) == true) {
	// 		printf("\n");
	// 		for (size_t p = 0; p < param_list[row].len; p++) { printf("%c",frac_raw[p + param_list[row].pos]); }
	// 		return &param_list[row];
	// 	}
	// }
	// printFlush("\nNo Results Error: Unable to find %s",path);
	// return NULL;

	// 			if (param_list[row].depth == depth) {
	// 			if (strictCompareText(&path[pos0], pos1 - pos0, &frac_raw[param_list[row].pos], param_list[row].len) == true) {
	// 				select = row;
	// 				printFlush("Matched row %" PRIu64,row);
	// 				break;
	// 			}
	// 		} else if (param_list[row].depth < depth) {
	// 			printFlush("\nDepth Error: Depth %" PRIu64 ", unable to find %s",depth,path);
	// 			return NULL;
	// 		}
	return NULL;
}

/* */
size_t getParameterArrayLength(const char* raw, const Param_List* item, const Param_List* list, size_t len) {
	if (len == 0) { fprintf(stderr, "Param_List* list length is 0"); return 0; }
	if (list == NULL) { fprintf(stderr, "Param_List* list is NULL"); return 0; }
	if (item == NULL) { fprintf(stderr, "Param_List* item is NULL"); return 0; }
	if (raw == NULL) { fprintf(stderr, "char* raw is NULL"); return 0; }
	size_t index = 0;
	for (; index < len; index++) { // Find the index of item
		if (list[index].pos == item->pos) {
			break;
		}
	}
	if (index == len) {
		fprintf(stderr, "Unable to find Param_List* item in Param_List* list");
		return 0;
	}
	size_t count = 0;
	for (; index < len; index++) { // Find the index of item
		if (list[index].depth == item->depth) {
			count++;
		} else {
			break;
		}
	}
	return count;
}


void copyHex(const char* raw, const Param_List* item, uint64_t* hex, size_t len) {
	if (raw == NULL || hex == NULL) {
		return;
	}
	if (item == NULL) {
		for (size_t i = 0; i < len; i++) {
			hex[i] = 0x0;
		}
		return;
	}
	if (item->len < 17 * len + 1) { // 64 + 5
		printFlushDebug("\nError: Hexdecimal code is not long enough (%" PRIu64 " < %" PRIu64 ")\n", item->len, 17 * len + 1);
		printParamTextN(raw,item);
		for (size_t i = 0; i < len; i++) {
			hex[i] = 0x0;
		}
		return;
	}
	const char* p = &raw[item->pos + 2]; // 0x
	for (size_t b = 0; b < len; b++) {
		hex[b] = 0;
		for (size_t x = 0; x < 16; x++) {
			uint8_t val = 0xFF;
			if (*p >= '0' && *p <= '9') {
				val = (uint8_t)(*p - '0');
			} else if (*p >= 'A' && *p <= 'F') {
				val = (uint8_t)(*p - 'A' + 10);
			} else if (*p >= 'a' && *p <= 'f') {
				val = (uint8_t)(*p - 'a' + 10);
			}
			if (val == 0xFF) {
				printFlushDebug("\nError: Invalid Hexadecimal Character\n");
				printParamTextN(raw,item);
				for (size_t i = 0; i < len; i++) {
					hex[i] = 0x0;
				}
				return;
			}
			hex[b] *= 16;
			hex[b] += val;
			p++;
		}
		p++;
	}
}

int correctWindowsFileName(char** ret, const char* path) {
	if (ret == NULL) {
		fprintf(stderr, "char** buf is NULL");
		return -1;
	}
	char* buf;
	size_t index = 0;
	size_t bufPtr = 0;
	while (path[index] != '\0') { // Calculates size
		if (index > 4096) {
			fprintf(stderr, "File path is too long");
			return -1;
		}
		if (path[index] == '\\') {
			bufPtr += 2;
		} else if (path[index] > ' ' || path[index] == '\0') { // Ignores control characters 1-31
			bufPtr++;
		}
		index++;
	}
	buf = (char*)calloc(bufPtr + 1, sizeof(char));
	if (buf == NULL) {
		fprintf(stderr, "Failed to allocate memory for file path");
		return -1;
	}
	*ret = buf;
	// Reset
	index = 0;
	bufPtr = 0;
	while (path[index] != '\0') {
		if (path[index] == '\\') {
						buf[bufPtr] = '\\'; bufPtr++;
			buf[bufPtr] = '\\'; bufPtr++;
		} else if (path[index] > ' ' || path[index] == '\0') { // Ignores control characters 1-31
			buf[bufPtr] = path[index]; bufPtr++;
		}
		index++;
	}
	buf[bufPtr] = '\0'; // Terminate String
	return 0;
}

int checkFileName(const char* input) {
	FILE *fptr;
	fptr = fopen(input, "r");
	if (fptr) {
		fclose(fptr);
		//printFlush("\nFile %s opened",input);
		return 0;
	} else {
		//printFlush("\nError: File %s does not exist or cannot be opened.",input);
		return -1;
	}
}

// Turns \ into \\ and removes any odd characters
int fixWindowsFileName(char** ret, const char* path) {
	if (ret == NULL) {
		fprintf(stderr, "char** buf is NULL");
		return -1;
	}
	char* buf;
	size_t index = 0;
	size_t bufPtr = 0;
	while (path[index] != '\0') { // Calculates size
		if (index > 4096) {
			fprintf(stderr, "File path is too long");
			return -1;
		}
		if (
			(path[index] >= 'a' && path[index] <= 'z') ||
			(path[index] >= 'A' && path[index] <= 'Z') ||
			(path[index] >= '0' && path[index] <= '9') ||
			(path[index] == ' ') || (path[index] == '-') || (path[index] == '_') || (path[index] == '/') || (path[index] == '.') ||
			(path[index] == '(') || (path[index] == ')') || (path[index] == '[') || (path[index] == ']') || (path[index] == '{') || (path[index] == '}') ||
			(path[index] == '<') || (path[index] == '>') ||
			(path[index] == '|') || (path[index] == '+') || (path[index] == '=') ||
			(path[index] == ';') || (path[index] == ':') || (path[index] == ',') || (path[index] == '`') || (path[index] == '~') ||
			(path[index] == '!') || (path[index] == '@') || (path[index] == '#') || (path[index] == '$') || (path[index] == '%') || (path[index] == '^') || (path[index] == '&')
		) {
			bufPtr++;
		} else if (path[index] == '\\') {
			bufPtr += 2;
		}
		index++;
	}
	buf = (char*)calloc(bufPtr + 1, sizeof(char));
	*ret = buf;
	if (buf == NULL) {
		fprintf(stderr, "Failed to allocate memory for file path");
		return -1;
	}
	// Resets
	index = 0;
	bufPtr = 0;
	while (path[index] != '\0') {
		if (
			(path[index] >= 'a' && path[index] <= 'z') ||
			(path[index] >= 'A' && path[index] <= 'Z') ||
			(path[index] >= '0' && path[index] <= '9') ||
			(path[index] == ' ') || (path[index] == '-') || (path[index] == '_') || (path[index] == '/') || (path[index] == '.') ||
			(path[index] == '(') || (path[index] == ')') || (path[index] == '[') || (path[index] == ']') || (path[index] == '{') || (path[index] == '}') ||
			(path[index] == '<') || (path[index] == '>') ||
			(path[index] == '|') || (path[index] == '+') || (path[index] == '=') ||
			(path[index] == ';') || (path[index] == ':') || (path[index] == ',') || (path[index] == '`') || (path[index] == '~') ||
			(path[index] == '!') || (path[index] == '@') || (path[index] == '#') || (path[index] == '$') || (path[index] == '%') || (path[index] == '^') || (path[index] == '&')
		) {
			buf[bufPtr] = path[index]; bufPtr++;
		} else if (path[index] == '\\') {
			buf[bufPtr] = '\\'; bufPtr++;
			buf[bufPtr] = '\\'; bufPtr++;
		}
		index++;
	}
	buf[bufPtr] = '\0'; // Terminate String
	return 0;
}
