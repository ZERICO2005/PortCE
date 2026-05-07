#include <sys/timers.h>
#include <sys/rtc.h>
#include <PortCE.h>
#include <cassert>

constexpr int SECS_PER_DAY  = 86400;
constexpr int SECS_PER_HOUR = 3600;
constexpr int SECS_PER_MIN  = 60;

/**
 * @remarks modified version of the CE mktime function
 */
static ce_time_t internal_mktime(
    int tm_sec,
    int tm_min,
    int tm_hour,
    int tm_mday,
    int tm_mon,
    int tm_year
) {
    static constexpr int yday[] = {
        -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333
    };
    assert(tm_mon >= 0 && tm_mon < 12 && "tm_mon out of range");

    return static_cast<ce_time_t>(
        tm_sec +
        tm_min * SECS_PER_MIN +
        tm_hour * SECS_PER_HOUR +
        (yday[tm_mon] + tm_mday) * SECS_PER_DAY +
        (tm_year - 70) * 31536000 + ((tm_year - 69) / 4) * SECS_PER_DAY -
        ((tm_year - 1) / 100) * SECS_PER_DAY + ((tm_year + 299) / 400) * SECS_PER_DAY
    );
}

ce_clock_t ce_clock(void) {
    return static_cast<ce_clock_t>(timer_Get(1));
}

double ce_difftime(ce_time_t end, ce_time_t beginning) {
    return static_cast<double>(static_cast<ez80_long>(end - beginning));
}

ce_time_t ce_time(ce_time_t *timer) {
    uint8_t sec, min, hrs;
    uint8_t day, month;
    uint16_t year;
    ce_time_t res;

    boot_GetDate(&day, &month, &year);
    boot_GetTime(&sec, &min, &hrs);

    res = internal_mktime(
        sec,
        min,
        hrs,
        day,
        month - 1,
        year - 1900
    );

    if (timer != nullptr) {
        *timer = res;
    }

    return res;
}
