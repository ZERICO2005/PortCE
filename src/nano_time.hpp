#ifndef NANO_TIME_HPP
#define NANO_TIME_HPP

#include <cstdint>
#include <ctime>
#include <cstdio>

typedef int64_t nano64_t;

static inline nano64_t getNanoTime(void) {
    struct timespec tp;
    if (clock_gettime(CLOCK_REALTIME, &tp) == 0) {
        nano64_t nanoTime = (nano64_t)tp.tv_sec * (nano64_t)1000000000 + (nano64_t)tp.tv_nsec;
        return nanoTime;
    } else {
        perror("clock_gettime");
        return 0;
    }
}

static inline double NANO_TO_SECONDS(nano64_t t) {
    return static_cast<double>(t) / 1.0e9;
}

static inline double NANO_TO_FRAMERATE(nano64_t t) {
    return 1.0e9 / static_cast<double>(t);
}

static inline nano64_t SECONDS_TO_NANO(double s) {
    return static_cast<nano64_t>(s * 1.0e9);
}

static inline double SECONDS_TO_FRAMERATE(double s) {
    return 1.0 / s;
}

static inline nano64_t FRAMERATE_TO_NANO(double f) {
    return static_cast<nano64_t>(1.0e9 / f);
}

static inline double FRAMERATE_TO_SECONDS(double f) {
    return 1.0 / f;
}

static inline double getDecimalTime(void) {
    return NANO_TO_SECONDS(getNanoTime());
}

#endif /* NANO_TIME_HPP */
