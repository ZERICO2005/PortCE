#include <ti/vars.h>

size_t os_MemChk(
    __attribute__((__unused__)) void **free
) {
    return 0;
}

void os_ArcChk(void) {
    return;
}

void *os_GetSymTablePtr(void) {
    return nullptr;
}

void *os_NextSymEntry(
    __attribute__((__unused__)) void *entry,
    __attribute__((__unused__)) uint24_t *type,
    __attribute__((__unused__)) uint24_t *nameLength,
    __attribute__((__unused__)) char *name,
    __attribute__((__unused__)) void **data
) {
    return nullptr;
}

int24_t os_DelSymEntry(
    __attribute__((__unused__)) void *entry
) {
    return -1;
}

int24_t os_CreateString(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) const string_t *data
) {
    return -1;
}

string_t *os_GetStringData(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t *archived
) {
    return nullptr;
}

int24_t os_CreateEquation(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) const equ_t *data
) {
    return -1;
}

equ_t *os_GetEquationData(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t *archived
) {
    return nullptr;
}

var_t *os_CreateAppVar(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) uint16_t size
) {
    return nullptr;
}

var_t *os_GetAppVarData(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t *archived
) {
    return nullptr;
}

void os_DelAppVar(
    __attribute__((__unused__)) const char *name
) {
    return;
}

int24_t os_ChkFindSym(
    __attribute__((__unused__)) uint8_t type,
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) void **entry,
    __attribute__((__unused__)) void **data
) {
    return 0;
}

int24_t os_GetVarSize(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) size_t *size
) {
    return -1;
}

int24_t os_GetMatrixDims(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t *rows,
    __attribute__((__unused__)) int24_t *cols
) {
    return -1;
}

int24_t os_GetRealListElement(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t index,
    __attribute__((__unused__)) real_t *value
) {
    return -1;
}

int24_t os_GetMatrixElement(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t row,
    __attribute__((__unused__)) int24_t col,
    __attribute__((__unused__)) real_t *value
) {
    return -1;
}

int24_t os_GetRealVar(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) real_t *value
) {
    return -1;
}

int24_t os_SetListDim(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t dim
) {
    return -1;
}

int24_t os_SetMatrixDims(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t rows,
    __attribute__((__unused__)) int24_t cols
) {
    return -1;
}

int24_t os_SetRealListElement(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t index,
    __attribute__((__unused__)) const real_t *value
) {
    return -1;
}
int24_t os_SetMatrixElement(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) int24_t row,
    __attribute__((__unused__)) int24_t col,
    __attribute__((__unused__)) const real_t *value
) {
    return -1;
}

int24_t os_SetRealVar(
    __attribute__((__unused__)) const char *name,
    __attribute__((__unused__)) const real_t *value
) {
    return -1;
}

void *os_GetAnsData(
    __attribute__((__unused__)) uint8_t *type
) {
    return nullptr;
}

void os_DelRes(void) {
    return;
}

int24_t os_RunPrgm(
    __attribute__((__unused__)) const char *prgm,
    __attribute__((__unused__)) void *data,
    __attribute__((__unused__)) size_t size,
    __attribute__((__unused__)) os_runprgm_callback_t callback
) {
    return -1;
}

int24_t os_Eval(
    __attribute__((__unused__)) const void *data,
    __attribute__((__unused__)) size_t len
) {
    return -1;
}

int24_t os_EvalVar(
    __attribute__((__unused__)) const char *name
) {
    return -1;
}
