#include "PortCE_internal.h"
#include <ti/real.h>

#include <string.h>
#include <cmath>
#include <stdio.h>
#include <inttypes.h>
#include <limits>

//------------------------------------------------------------------------------
// Config
//------------------------------------------------------------------------------

/**
 * ti_real     : effectively ~46.5 bits of mantissa
 * long double : represents larger integers exactly (useful for nCr and nPr)
 * double      : more portable 53 bits of mantissa should be sufficient
 * float       : not enough range or precision
 */

// If you need to use double or etc
typedef double long_double;

// "L" for long double, "l" for double, "" for float
#define PRI_LD "l"

// strtold for long double, strtod for double, strtof for float
#define strto_float strtod

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

constexpr long_double max_real = long_double(9.9999999999999e99L);
constexpr long_double min_real = long_double(1.0e-99L);

//------------------------------------------------------------------------------
// Exact Functions
//------------------------------------------------------------------------------

real_t os_RealCopy(const real_t *src) {
    real_t dst;
    memcpy(&dst, src, sizeof(real_t));
    dst.sign &= 0x7F;
    return dst;
}

real_t os_RealNeg(const real_t *arg) {
    real_t ret;
    memcpy(&ret, arg, sizeof(real_t));
    if (ret.mant[0] == 0) {
        ret.sign |= 0x80;
    }
    ret.sign ^= 0x80;
    return ret;
}

//------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------

union ti_mant {
    uint64_t digits;
    struct {
        uint8_t d13 : 4;
        uint8_t d12 : 4;
        uint8_t d11 : 4;
        uint8_t d10 : 4;
        uint8_t d9  : 4;
        uint8_t d8  : 4;
        uint8_t d7  : 4;
        uint8_t d6  : 4;
        uint8_t d5  : 4;
        uint8_t d4  : 4;
        uint8_t d3  : 4;
        uint8_t d2  : 4;
        uint8_t d1  : 4;
        uint8_t d0  : 4;
    };
    uint8_t mant[7];
};

static real_t set_sign(real_t x, bool sign) {
    x.sign = sign ? 0x80 : 0x00;
    return x;
}

static real_t get_zero_real_t() {
    real_t x;
    memset(&x, 0, sizeof(real_t));
    x.exp = 0x80;
    return x;
}

static real_t get_min_real_t(bool sign = false) {
    real_t x;
    memset(&x, 0, sizeof(real_t));
    x.exp = 0x1D;
    x.mant[0] = 0x10;
    return set_sign(x, sign);
}

static real_t get_max_real_t(bool sign = false) {
    real_t x;
    memset(&x, 0, sizeof(real_t));
    x.exp = 0xE3;
    x.mant[0] = 0x99;
    x.mant[1] = 0x99;
    x.mant[2] = 0x99;
    x.mant[3] = 0x99;
    x.mant[4] = 0x99;
    x.mant[5] = 0x99;
    x.mant[6] = 0x99;
    return set_sign(x, sign);
}

static bool test_long_double_string(const char* buf) {
    // test for invalid output
    if (memchr(buf, '\0', sizeof("9.9999999999999e+0100")) == nullptr) {
        return false;
    }
    if (!(buf[0] >= '0' && buf[0] <= '9')) {
        return false;
    }
    if (buf[1] != '.') {
        return false;
    }
    // we probably do not need to test to that extent
    return true;
}

static real_t long_double_to_real(long_double arg) {
    const bool sign = (bool)std::signbit(arg);
    // get NaN out of the way first to avoid problems
    if (std::isnan(arg)) {
        return get_zero_real_t();
    }
    const long_double val = std::fabs(arg);
    // test if larger than std::numeric_limits::max()
    if (std::isinf(val) || val >= max_real) {
        return get_max_real_t(sign);
    }
    // test if smaller than std::numeric_limits::min()
    if (val <= min_real) {
        // rounds to zero
        if (val == long_double(0.0L) || val < min_real / long_double(2.0L)) {
            return get_zero_real_t();
        }
        // rounds to std::numeric_limits::min()
        return get_min_real_t(sign);
    }
    char buf[100];
    snprintf(buf, sizeof(buf), "%.13" PRI_LD "e\n", val);
    if (!test_long_double_string(buf)) {
        fprintf(stderr, "failed to convert long double to real_t (snprintf)\n");
        return get_zero_real_t();
    }
    uint64_t leading_digit;
    ti_mant hex;
    // make sure the whole struct/union is zero
    memset(&hex, 0, sizeof(hex));
    int exponent;
    int status = sscanf(buf,
        "%1" SCNu64 ".%13" SCNx64 "%*1[eE]%d",
        &leading_digit, &hex.digits, &exponent
    );
    if (status != 3) {
        fprintf(stderr, "failed to convert long double to real_t (sscanf %d)\n", status);
        return get_zero_real_t();
    }
    hex.digits |= leading_digit << (13 * 4);
    real_t ret;
    memset(&ret, 0, sizeof(ret));
    ret.mant[6] = hex.mant[0];
    ret.mant[5] = hex.mant[1];
    ret.mant[4] = hex.mant[2];
    ret.mant[3] = hex.mant[3];
    ret.mant[2] = hex.mant[4];
    ret.mant[1] = hex.mant[5];
    ret.mant[0] = hex.mant[6];
    ret.exp = (int8_t)(exponent + 0x80);
    set_sign(ret, sign);
    #if 0
        printf(
            "%016llx "
            "%u%u%u%u%u%u%u%u%u%u%u%u%u%u "
            "%02x%02x%02x%02x%02x%02x%02x\n",
            digits,
            d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13,
            mant[6], mant[5], mant[4], mant[3], mant[2], mant[1], mant[0]
        );
    #endif
    return ret;
}

static long_double real_to_long_double(real_t val) {
    const bool sign = (val.sign & 0x80);
    int expon = (int)((uint8_t)val.exp) - 0x80;
    ti_mant hex;
    // make sure the whole struct/union is zero
    memset(&hex, 0, sizeof(hex));

    hex.mant[6] = val.mant[0];
    hex.mant[5] = val.mant[1];
    hex.mant[4] = val.mant[2];
    hex.mant[3] = val.mant[3];
    hex.mant[2] = val.mant[4];
    hex.mant[1] = val.mant[5];
    hex.mant[0] = val.mant[6];

    while ((hex.d0 == 0) && hex.digits != 0) {
        // shift by 1 digit
        hex.digits <<= 4;
        expon--;
    }
    if (hex.digits == 0) {
        return sign ? long_double(+0.0L) : long_double(-0.0L);
    }
    uint64_t decimal = 0;
    decimal += hex.d13 * UINT64_C(1);
    decimal += hex.d12 * UINT64_C(10);
    decimal += hex.d11 * UINT64_C(100);
    decimal += hex.d10 * UINT64_C(1000);
    decimal += hex.d9  * UINT64_C(10000);
    decimal += hex.d8  * UINT64_C(100000);
    decimal += hex.d7  * UINT64_C(1000000);
    decimal += hex.d6  * UINT64_C(10000000);
    decimal += hex.d5  * UINT64_C(100000000);
    decimal += hex.d4  * UINT64_C(1000000000);
    decimal += hex.d3  * UINT64_C(10000000000);
    decimal += hex.d2  * UINT64_C(100000000000);
    decimal += hex.d1  * UINT64_C(1000000000000);
    decimal += hex.d0  * UINT64_C(10000000000000);
    long_double ret = (long_double)decimal;
    ret *= std::pow(long_double(10.0L), (long_double)(expon - 13));
    if (std::isnan(ret)) {
        ret = long_double(0.0L);
    }
    if (std::isinf(ret)) {
        ret = std::numeric_limits<long_double>::max();
    }
    if (sign) {
        ret = -ret;
    }
    return ret;
}

//------------------------------------------------------------------------------
// Approximate Functions
//------------------------------------------------------------------------------

real_t os_RealAcosRad(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::acos(val);
    return long_double_to_real(ret);
}

real_t os_RealAsinRad(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::asin(val);
    return long_double_to_real(ret);
}

real_t os_RealAtanRad(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::atan(val);
    return long_double_to_real(ret);
}

real_t os_RealCosRad(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::cos(val);
    return long_double_to_real(ret);
}

real_t os_RealRadToDeg(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    constexpr long_double constant_180_div_pi = long_double(57.295779513082320876798154814105L);
    long_double ret = val * constant_180_div_pi;
    return long_double_to_real(ret);
}

real_t os_RealExp(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::exp(val);
    return long_double_to_real(ret);
}

real_t os_RealFloor(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::floor(val);
    return long_double_to_real(ret);
}

real_t os_RealFrac(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double temp;
    long_double ret = std::modf(val, &temp);
    return long_double_to_real(ret);
}

real_t os_RealRoundInt(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::round(val);
    return long_double_to_real(ret);
}

real_t os_RealLog(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::log(val);
    return long_double_to_real(ret);
}

real_t os_RealDegToRad(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    constexpr long_double constant_pi_div_180 = long_double(0.01745329251994329576923690768489L);
    long_double ret = val * constant_pi_div_180;
    return long_double_to_real(ret);
}

real_t os_RealInv(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = long_double(1.0L) / val;
    return long_double_to_real(ret);
}

real_t os_RealSinRad(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::sin(val);
    return long_double_to_real(ret);
}

real_t os_RealSqrt(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::sqrt(val);
    return long_double_to_real(ret);
}

real_t os_RealTanRad(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::tan(val);
    return long_double_to_real(ret);
}

real_t os_RealInt(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    long_double ret = std::trunc(val);
    return long_double_to_real(ret);
}

cplx_t os_CplxSquare(const cplx_t *arg) {
    long_double x = real_to_long_double(arg->real);
    long_double y = real_to_long_double(arg->imag);
    long_double ret_real = (x * x) - (y * y);
    long_double ret_imag = long_double(2.0L) * x * y;
    cplx_t ret;
    ret.real = long_double_to_real(ret_real);
    ret.imag = long_double_to_real(ret_imag);
    return ret;
}

real_t os_RealAdd(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    long_double ret = x + y;
    return long_double_to_real(ret);
}

real_t os_RealDiv(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    long_double ret = x / y;
    return long_double_to_real(ret);
}

static long_double math_gcd(long_double x, long_double y) {
    while (y != long_double(0.0L)) {
        long_double r = std::fmod(x, y);
        x = y;
        y = r;
    }
    return x;
}

real_t os_RealGcd(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    if (std::trunc(x) != x || std::trunc(y) != y) {
        // non integer arguments
        return get_zero_real_t();
    }
    x = std::abs(x);
    y = std::abs(y);

    long_double ret = math_gcd(x, y);

    return long_double_to_real(ret);
}

real_t os_RealLcm(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    if (std::trunc(x) != x || std::trunc(y) != y) {
        // non integer arguments
        return get_zero_real_t();
    }
    if (x == long_double(0.0L) || y == long_double(0.0L)) {
        return get_zero_real_t();
    }

    long_double ret = std::abs((x * y) / math_gcd(x, y));
    return long_double_to_real(ret);
}

real_t os_RealMax(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    long_double ret = std::fmax(x, y);
    return long_double_to_real(ret);
}

real_t os_RealMin(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    long_double ret = std::fmin(x, y);
    return long_double_to_real(ret);
}

real_t os_RealMul(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    long_double ret = x * y;
    return long_double_to_real(ret);
}

static long_double math_factorial(long_double x) {
    return std::tgamma(x + long_double(1.0L));
}

real_t os_RealNcr(const real_t *total, const real_t *num) {
    long_double n = real_to_long_double(*total);
    long_double r = real_to_long_double(*num);
    long_double ret = math_factorial(n) / (math_factorial(n - r) * math_factorial(r));
    return long_double_to_real(ret);
}

real_t os_RealNpr(const real_t *total, const real_t *num) {
    long_double n = real_to_long_double(*total);
    long_double r = real_to_long_double(*num);
    long_double ret = math_factorial(n) / math_factorial(n - r);
    return long_double_to_real(ret);
}

real_t os_RealPow(const real_t *base, const real_t *exp) {
    long_double x = real_to_long_double(*base);
    long_double y = real_to_long_double(*exp);
    long_double ret = std::pow(x, y);
    return long_double_to_real(ret);
}

real_t os_RealRandInt(__attribute__((__unused__)) const real_t *min, __attribute__((__unused__)) const real_t *max) {
    return get_zero_real_t();
}

real_t os_RealMod(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    long_double ret = std::fmod(x, y);
    return long_double_to_real(ret);
}

real_t os_RealSub(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    long_double ret = x - y;
    return long_double_to_real(ret);
}

real_t os_RealRound(const real_t *arg, char digits) {
    long_double val = real_to_long_double(*arg);
    long_double scale = std::pow(long_double(10.0L), static_cast<long_double>(digits));
    val *= scale;
    val = std::round(val);
    val /= scale;
    return long_double_to_real(val);
}

int os_RealCompare(const real_t *arg1, const real_t *arg2) {
    long_double x = real_to_long_double(*arg1);
    long_double y = real_to_long_double(*arg2);
    if (x < y) {
        return -1;
    }
    if (x > y) {
        return +1;
    }
    return 0;
}

int24_t os_RealToInt24(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    if (val > static_cast<long_double>(std::numeric_limits<int24_t>::max())) {
        return std::numeric_limits<int24_t>::max();
    }
    if (val < static_cast<long_double>(std::numeric_limits<int24_t>::lowest())) {
        return std::numeric_limits<int24_t>::lowest();
    }
    return static_cast<int24_t>(val);
}

real_t os_Int24ToReal(int24_t arg) {
    long_double val = static_cast<long_double>(arg);
    return long_double_to_real(val);
}

float os_RealToFloat(const real_t *arg) {
    long_double val = real_to_long_double(*arg);
    if (val > static_cast<long_double>(std::numeric_limits<float>::max())) {
        return std::numeric_limits<float>::max();
    }
    if (val < static_cast<long_double>(std::numeric_limits<float>::lowest())) {
        return std::numeric_limits<float>::lowest();
    }
    return static_cast<float>(val);
}

real_t os_FloatToReal(float arg) {
    long_double val = static_cast<long_double>(arg);
    return long_double_to_real(val);
}

int os_RealToStr(char *result, const real_t *arg, int8_t maxLength, uint8_t mode, int8_t digits) {
    if (maxLength <= 0) {
        maxLength = 14;
    }
    if (maxLength < 6) {
        maxLength = 6;
    }
    if (digits < -1) {
        digits = -1;
    }
    if (digits > 9) {
        digits = 9;
    }
    if (mode > 4) {
        mode = 4;
    }

    long_double val = real_to_long_double(*arg);
    const char* format;
    const char* format_scientific = "%.*" PRI_LD "e";
    const char* format_regular = "%.*" PRI_LD "f";
    const char* format_compact = "%.*" PRI_LD "g";
    switch (mode) {
        case 0: format = format_regular; break;
        case 1: format = format_regular; break;
        case 2: format = format_scientific; break;
        case 3: format = format_scientific; break;
        default:
        case 4: format = format_regular; break;
    }
    memset(result, '\0', (size_t)maxLength + 1);
    if (std::fabs(val) < long_double(1.0e-3L) || std::fabs(val) >= long_double(1.0e+11L)) {
        format = format_scientific;
    }

    int len, ret;
    len = snprintf(result, (size_t)maxLength + 1, format, digits, val);
    ret = len - 1;
    if (len <= maxLength && len > 0) {
        // assuming this returns strlen
        return ret;
    }

    // try again with a different format
    format = format_compact;
    len = snprintf(result, (size_t)maxLength + 1, format, digits, val);
    ret = len - 1;
    if (len <= maxLength && len > 0) {
        // assuming this returns strlen
        return ret;
    }

    // failed
    ret = 0;
    if (std::signbit(val)) {
        *result++ = '-'; ret++;
    }
    *result++ = '0'; ret++;
    *result++ = '\0';
    return ret;
}

real_t os_StrToReal(const char *string, char **end) {
    long_double val = strto_float(string, end);
    return long_double_to_real(val);
}
