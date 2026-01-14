#include "PortCE_internal.h"
#include <ti/real.h>

#include <string.h>

static real_t get_zero_real_t() {
    real_t x;
    memset(&x, 0, sizeof(real_t));
    return x;
}

static cplx_t get_zero_cplx_t() {
    cplx_t x;
    memset(&x, 0, sizeof(cplx_t));
    return x;
}

real_t os_RealCopy(const real_t *src) {
    real_t dst;
    memcpy(&dst, src, sizeof(real_t));
    dst.sign &= 0x7F;
    return get_zero_real_t();
}

real_t os_RealAcosRad(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealAsinRad(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealAtanRad(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealCosRad(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealRadToDeg(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealExp(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealFloor(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealFrac(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealRoundInt(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealLog(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealNeg(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealDegToRad(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealInv(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealSinRad(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealSqrt(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealTanRad(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

real_t os_RealInt(__attribute__((unused)) const real_t *arg) {
    return get_zero_real_t();
}

cplx_t os_CplxSquare(__attribute__((unused)) const cplx_t *arg) {
    return get_zero_cplx_t();
}

real_t os_RealAdd(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealDiv(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealGcd(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealLcm(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealMax(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealMin(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealMul(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealNcr(__attribute__((unused)) const real_t *total, __attribute__((unused)) const real_t *num) {
    return get_zero_real_t();
}

real_t os_RealNpr(__attribute__((unused)) const real_t *total, __attribute__((unused)) const real_t *num) {
    return get_zero_real_t();
}

real_t os_RealPow(__attribute__((unused)) const real_t *base, __attribute__((unused)) const real_t *exp) {
    return get_zero_real_t();
}

real_t os_RealRandInt(__attribute__((unused)) const real_t *min, __attribute__((unused)) const real_t *max) {
    return get_zero_real_t();
}

real_t os_RealMod(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealSub(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return get_zero_real_t();
}

real_t os_RealRound(__attribute__((unused)) const real_t *arg, __attribute__((unused)) char digits) {
    return get_zero_real_t();
}

int os_RealCompare(__attribute__((unused)) const real_t *arg1, __attribute__((unused)) const real_t *arg2) {
    return 0;
}

int24_t os_RealToInt24(__attribute__((unused)) const real_t *arg) {
    return 0;
}

real_t os_Int24ToReal(__attribute__((unused)) int24_t arg) {
    return get_zero_real_t();
}

float os_RealToFloat(__attribute__((unused)) const real_t *arg) {
    return 0.0f;
}

real_t os_FloatToReal(__attribute__((unused)) float arg) {
    return get_zero_real_t();
}

int os_RealToStr(char *result, __attribute__((unused)) const real_t *arg, int8_t maxLength, __attribute__((unused)) uint8_t mode, __attribute__((unused)) int8_t digits) {
    if (maxLength <= 0) {
        maxLength = 14;
    }
    if (maxLength < 6) {
        maxLength = 6;
    }
    memset(result, '\0', maxLength + 1);
    *result = '0';
    return maxLength;
}

real_t os_StrToReal(const char *string, char **end) {
    if (end != nullptr) {
        *end = (char*)string;
    }
    return get_zero_real_t();
}
