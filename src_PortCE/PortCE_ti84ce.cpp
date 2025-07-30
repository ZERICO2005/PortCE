/**
 * @file
 *
 * @author "zerico2005"
 */

/*
**  PortCE_ti84ce.h simulates libraries and functions from the CE Programming Toolchain.
**  Some functions may not be completely accurate or fully implemented.
*/

#include "PortCE_Common.h"

#include "PortCE_Render.h"
#include "PortCE_include/ce/include/sys/lcd.h"
#include "PortCE_include/ce/include/sys/util.h"
#include "PortCE_include/ce/include/sys/timers.h"
#include "PortCE_include/ce/include/sys/rtc.h"
#include "PortCE_include/lcddrvce/lcddrvce.h"

#include "PortCE_include/ce/include/tice.h"
#include "PortCE_include/fileioc/fileioc.h"

// #include <process.h>
//#include <dir.h>
// #include <conio.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <unistd.h>

extern uint8_t simulated_ram[16777216];

uint16_t timer_IntAcknowledge;
static uint16_t* const timer_IntStatus_ptr = (uint16_t*)((void*)&simulated_ram[0xF20034]);

uint8_t rtc_IntAcknowledge;
static uint8_t* const rtc_IntStatus_ptr = (uint8_t*)((void*)&simulated_ram[0xF30034]);

/* <sys/util.h> */

uint32_t random(void) {
    uint32_t ret = 0;
    ret <<= 15;
    ret ^= rand();
    ret <<= 15;
    ret ^= rand();
    ret <<= 15;
    ret ^= rand();
    return ret;
}

void srandom(uint32_t seed) {
    srand(seed);
}

/** Not actually atomic */
uint32_t atomic_load_32(volatile uint32_t* p) {
    return *p;
}

/** Not actually atomic */
uint32_t atomic_load_increasing_32(volatile uint32_t* p) {
    uint32_t read_0 = *p;
    uint32_t read_1 = *p;
    return std::max(read_0, read_1);
}

/** Not actually atomic */
uint32_t atomic_load_decreasing_32(volatile uint32_t* p) {
    uint32_t read_0 = *p;
    uint32_t read_1 = *p;
    return std::min(read_0, read_1);
}


/* <ti/debug.h> */

    void boot_DebugPrintf(const char *string) {
        printf("%s",string);
    }

/* <sys/rtc.h> */

    static uint8_t  calc_day     = 1   ;
    static uint8_t  calc_month   = 1   ;
    static uint16_t calc_year    = 2015;
    static uint8_t  calc_hours   = 0   ;
    static uint8_t  calc_minutes = 0   ;
    static uint8_t  calc_seconds = 0   ;

    /** @todo Add proper date validation */
    void boot_SetDate(uint8_t day, uint8_t month, uint16_t year) {
        if (day > 31 || month > 12 || year < 2015) {
            return;
        }
        calc_year  = year ;
        calc_month = month;
        calc_day   = day  ;
    }

    void boot_GetDate(uint8_t *day, uint8_t *month, uint16_t *year) {
        if (day   != NULL) { *day   = calc_day  ; }
        if (month != NULL) { *month = calc_month; }
        if (year  != NULL) { *year  = calc_year ; }
    }

    void boot_SetTime(uint8_t seconds, uint8_t minutes, uint8_t hours) {
        if (seconds >= 60 || minutes >= 60 || hours >= 24) {
            return;
        }
        calc_seconds = seconds;
        calc_minutes = minutes;
        calc_hours   = hours  ;
    }

    void boot_GetTime(uint8_t *seconds, uint8_t *minutes, uint8_t *hours) {
        if (seconds != NULL) { *seconds = calc_seconds; }
        if (minutes != NULL) { *minutes = calc_minutes; }
        if (hours   != NULL) { *hours   = calc_hours  ; }
    }

    bool boot_IsAfterNoon(void) {
        return (calc_hours >= 12) ? true : false;
    }

/* <sys/timers.h> */

    // struct Simulated_Timer {
    //     uint8_t enabled;
    //     uint8_t rate;
    //     uint8_t inter;
    //     uint8_t dir;
    // }; typedef struct Simulated_Timer Simulated_Timer;

    // static Simulated_Timer simulated_timer[3] = {
    //     {0,1,0,1},{1,1,0,1},{1,1,0,1}
    // };

    // void timer_Enable(uint8_t n, uint8_t rate, uint8_t inter, uint8_t dir) {
    //     if (n < 1 || n > 3) { return; }
    //     simulated_timer[n - 1].enabled = 1;
    //     simulated_timer[n - 1].rate = rate;
    //     simulated_timer[n - 1].inter = inter;
    //     simulated_timer[n - 1].dir = dir;
    // }

    // void timer_Disable(uint8_t n) {
    //     if (n < 1 || n > 3) { return; }
    //     simulated_timer[n - 1].enabled = 0;
    // }

    // // n is supposed to be a value between 1 and 3
    // uint32_t timer_Get(uint8_t n) {
    //     if (n < 1 || n > 3) { return 0; }
    //     if (simulated_timer[n - 1].enabled == 0) {
    //         return 0;
    //     }
    //     if (simulated_timer[n - 1].rate == 0) {
    //         double timerCPU = getDecimalTime(); //Time in seconds
    //         timerCPU *= 15000000.0;
    //         return (uint32_t)timerCPU;
    //     }
    //     double timer32K = getDecimalTime(); //Time in seconds
    //     timer32K *= 32768.0;
    //     return (uint32_t)timer32K;
    // }

    /* Sleep and Delays */

        void delay(uint16_t msec) {
            nano64_t dur = (nano64_t)msec * 1000000;
            nano64_t startTime = getNanoTime();
            PortCE_new_frame();
            while (getNanoTime() - startTime < dur) {
                /* Wait */
            }
        }

        void msleep(uint16_t msec) {
            nano64_t dur = (nano64_t)msec * 1000000;
            nano64_t startTime = getNanoTime();
            PortCE_new_frame();
            while (getNanoTime() - startTime < dur) {
                /* Wait */
            }
        }

        ti_unsigned_int sleep(ti_unsigned_int seconds) {
            nano64_t dur = (nano64_t)seconds * 1000000000;
            nano64_t startTime = getNanoTime();
            while (getNanoTime() - startTime < dur) {
                PortCE_new_frame();
            }
            return 0;
        }

        void ticksleep(ti_unsigned_long ticks) {
            nano64_t dur = (nano64_t)((double)ticks * (1000000000.0 / (double)TI_CLOCKS_PER_SEC));
            nano64_t startTime = getNanoTime();
            PortCE_new_frame();
            while (getNanoTime() - startTime < dur) {
                /* Wait */
            }
        }

        ti_int usleep(useconds_t usec) {
            nano64_t dur = (nano64_t)usec * 1000;
            nano64_t startTime = getNanoTime();
            PortCE_new_frame();
            while (getNanoTime() - startTime < dur) {
                /* Wait */
            }
            return 0;
        }

        void boot_WaitShort(void) {
            delay(10);
        }

/* Update */

// typedef struct Ti_Timer {
//     uint32_t Counter;
//     uint32_t ReloadValue;
//     uint32_t MatchValue_1;
//     uint32_t MatchValue_2;
// } Ti_Timer;

// static Ti_Timer* const timer_list = (Ti_Timer*)((void*)&simulated_ram[0xF20000]);
// static const size_t timer_count = 3;

static const double timer_mult = 1.0;

ti_clock_t ti_clock() {
    return (ti_clock_t)((double)clock() * ((double)TI_CLOCKS_PER_SEC / (double)CLOCKS_PER_SEC) * timer_mult);
}

static nano64_t last_timer_update = 0;

typedef struct Timer_Struct {
    uint32_t Counter;
    uint32_t ReloadValue;
    uint32_t MatchValue_1;
    uint32_t MatchValue_2;
} Timer_Struct;

__attribute__((unused)) static const uint16_t Timer_ENABLE [3] = {(1<<0), (1<<3 ), (1<<6 )};
__attribute__((unused)) static const uint16_t Timer_DISABLE[3] = {(0<<0), (0<<3 ), (0<<6 )};
__attribute__((unused)) static const uint16_t Timer_32K    [3] = {(1<<1), (1<<4 ), (1<<7 )};
__attribute__((unused)) static const uint16_t Timer_CPU    [3] = {(0<<1), (0<<4 ), (0<<7 )};
__attribute__((unused)) static const uint16_t Timer_0INT   [3] = {(1<<2), (1<<5 ), (1<<8 )};
__attribute__((unused)) static const uint16_t Timer_NOINT  [3] = {(0<<2), (0<<5 ), (0<<8 )};
__attribute__((unused)) static const uint16_t Timer_UP     [3] = {(1<<9), (1<<10), (1<<11)};
__attribute__((unused)) static const uint16_t Timer_DOWN   [3] = {(0<<9), (0<<10), (0<<11)};

__attribute__((unused)) static const uint16_t Timer_MATCH1  [3] = {(1<<0), (1<<3), (1<<6)};
__attribute__((unused)) static const uint16_t Timer_MATCH2  [3] = {(1<<1), (1<<4), (1<<7)};
__attribute__((unused)) static const uint16_t Timer_RELOADED[3] = {(1<<2), (1<<5), (1<<8)};

static Timer_Struct* const timer_list = (Timer_Struct*)((void*)&simulated_ram[0xF20000]);

// accounts for the clock wrapping around
static bool is_between_times(uint32_t time, uint32_t t0, uint32_t t1, bool forwards) {
    // WARNING: This may have fencepost errors
    if (forwards) {
        // t0 <= t1
        time -= t0;
        t1 -= t0;
        if (0 <= time && time <= t1) {
            return true;
        }
        return false;
    } else {
        // t1 <= t0
        time -= t1;
        t0 -= t1;
        if (0 <= time && time <= t0) {
            return true;
        }
        return false;
    }
}

static bool did_timer_overflow(uint32_t t0, uint32_t t1, bool forwards) {
    if (t0 == 0) {
        return false;
    }
    return is_between_times(0, t0, t1, forwards);
}

static void PortCE_update_timers(void) {
    if (last_timer_update == 0) {
        last_timer_update = getNanoTime();
    }
    const nano64_t current_time = getNanoTime();

    const nano64_t delta_nano = (current_time - last_timer_update);
    const uint32_t delta_32K = (uint32_t)((int32_t)((double)delta_nano * (32768.0 / 1.0e9) * timer_mult)); // 32 KHz
    const uint32_t delta_CPU = (uint32_t)((int32_t)((double)delta_nano * (Ti84CE_Clockspeed / 1.0e9) * timer_mult)); // 8 MHz
    if (delta_32K == 0 || delta_CPU == 0) {
        // Prevents infinite loops if not enough time passes between updates
        return;
    }

    /* Update timers */

    // Clear acknowledged interrupt bits
    *timer_IntStatus_ptr &= ~timer_IntAcknowledge;
    timer_IntAcknowledge = 0;

    for (int i = 0; i < 3; i++) {
        if (!(timer_Control & Timer_ENABLE[i])) {
            // timer is disabled
            // continue;
        }
        uint32_t prev_Counter = timer_list[i].Counter;
        bool forwards_clock = (timer_Control & Timer_UP[i]);
        uint32_t timer_increment = (timer_Control & Timer_32K[i]) ? delta_32K : delta_CPU;
        timer_list[i].Counter += forwards_clock ? timer_increment : -timer_increment;

        if (is_between_times(
            timer_list[i].MatchValue_1, prev_Counter, timer_list[i].Counter, forwards_clock
        )) {
            *timer_IntStatus_ptr |= Timer_MATCH1[i];
        }
        if (is_between_times(
            timer_list[i].MatchValue_2, prev_Counter, timer_list[i].Counter, forwards_clock
        )) {
            *timer_IntStatus_ptr |= Timer_MATCH2[i];
        }
        bool zero_interrupt = timer_Control & Timer_0INT[i];
        if (zero_interrupt && did_timer_overflow(prev_Counter, timer_list[i].Counter, forwards_clock)) {
            *timer_IntStatus_ptr |= Timer_RELOADED[i];
            timer_list[i].Counter = timer_list[i].ReloadValue;
        }
    }

    #if 0
    /* Update timers */
        if (timer_Control & TIMER1_ENABLE) {
            if (timer_Control & TIMER1_UP) {
                timer_1_Counter += (timer_Control & TIMER1_32K) ? delta_32K : delta_CPU;

            } else {
                timer_1_Counter -= (timer_Control & TIMER1_32K) ? delta_32K : delta_CPU;
            }
        }
        if (timer_Control & TIMER2_ENABLE) {
            if (timer_Control & TIMER2_UP) {
                timer_2_Counter += (timer_Control & TIMER2_32K) ? delta_32K : delta_CPU;
            } else {
                timer_2_Counter -= (timer_Control & TIMER2_32K) ? delta_32K : delta_CPU;
            }
        }
        if (timer_Control & TIMER3_ENABLE) {
            if (timer_Control & TIMER3_UP) {
                timer_3_Counter += (timer_Control & TIMER3_32K) ? delta_32K : delta_CPU;
            } else {
                timer_3_Counter -= (timer_Control & TIMER3_32K) ? delta_32K : delta_CPU;
            }
        }
    #endif

    last_timer_update = current_time;
}

static void get_RTC_time(uint8_t* Seconds, uint8_t* Minutes, uint8_t* Hours, uint16_t* Days) {
    *Seconds = 0;
    *Minutes = 0;
    *Hours = 0;
    *Days = 0;

    time_t now;
    struct tm *tm;
    now = time(NULL);
    if (now == (time_t)-1) {
        // unable to retrive time
        return;
    }

    tm = localtime(&now);
    if (tm == NULL) {
        // unable to convert time
        return;
    }

    // Populate the time values
    *Seconds = (uint8_t)tm->tm_sec;
    *Minutes = (uint8_t)tm->tm_min;
    *Hours   = (uint8_t)tm->tm_hour;
    *Days    = (uint8_t)tm->tm_yday;
}

static uint32_t* const RTC_Time    = (uint32_t*)((void*)&simulated_ram[0xF30044]);
static uint8_t*  const RTC_Seconds = (uint8_t* )((void*)&simulated_ram[0xF30000]); /**< RTC seconds */
static uint8_t*  const RTC_Minutes = (uint8_t* )((void*)&simulated_ram[0xF30004]); /**< RTC minutes */
static uint8_t*  const RTC_Hours   = (uint8_t* )((void*)&simulated_ram[0xF30008]); /**< RTC hours */
static uint16_t* const RTC_Days    = (uint16_t*)((void*)&simulated_ram[0xF3000C]); /**< RTC days */

static uint8_t* const RTC_AlarmSeconds = (uint8_t*)((void*)&simulated_ram[0xF30010]);    /**< RTC alarm seconds */
static uint8_t* const RTC_AlarmMinutes = (uint8_t*)((void*)&simulated_ram[0xF30014]);    /**< RTC alarm minutes */
static uint8_t* const RTC_AlarmHours   = (uint8_t*)((void*)&simulated_ram[0xF30018]);    /**< RTC alarm hours */

// Doesn't properly support internal counter/latching
static void PortCE_update_RTC(void) {
    uint8_t Seconds, Minutes, Hours;
    uint16_t Days;
    get_RTC_time(&Seconds, &Minutes, &Hours, &Days);

    *rtc_IntStatus_ptr &= ~rtc_IntAcknowledge;
    rtc_IntAcknowledge = 0;

    if ((rtc_Control & (1<<0)) && (rtc_Control & (RTC_UNFREEZE))) {
        const bool change_Seconds = (*RTC_Seconds != Seconds);
        const bool change_Minutes = (*RTC_Minutes != Minutes);
        const bool change_Hours   = (*RTC_Hours   != Hours  );
        const bool change_Days    = (*RTC_Days    != Days   );

        if (change_Seconds && (rtc_Control & RTC_SEC_INT_SOURCE)) { *rtc_IntStatus_ptr |= RTC_SEC_INT; }
        if (change_Minutes && (rtc_Control & RTC_MIN_INT_SOURCE)) { *rtc_IntStatus_ptr |= RTC_MIN_INT; }
        if (change_Hours   && (rtc_Control & RTC_HR_INT_SOURCE )) { *rtc_IntStatus_ptr |= RTC_HR_INT ; }
        if (change_Days    && (rtc_Control & RTC_DAY_INT_SOURCE)) { *rtc_IntStatus_ptr |= RTC_DAY_INT; }

        if (rtc_Control & RTC_ALARM_INT_SOURCE) {
            if (change_Seconds && *RTC_AlarmSeconds == Seconds) { *rtc_IntStatus_ptr |= RTC_ALARM_INT; }
            if (change_Minutes && *RTC_AlarmMinutes == Minutes) { *rtc_IntStatus_ptr |= RTC_ALARM_INT; }
            if (change_Hours   && *RTC_AlarmHours   == Hours  ) { *rtc_IntStatus_ptr |= RTC_ALARM_INT; }
        }

        *RTC_Seconds = Seconds;
        *RTC_Minutes = Minutes;
        *RTC_Hours   = Hours  ;
        *RTC_Days    = Days   ;
        *RTC_Time = (Seconds & 0x3F) | ((Minutes & 0x3F) >> 6) | ((Hours & 0x1F) >> 12) | ((Days & 0x7FFF) >> 17);
    }
}

ti_int PortCE_update_registers(void) {
    PortCE_update_timers();
    PortCE_update_RTC();
    return 0;
}

/* Setup */

void reset_ti84ce_registers(void) {

    lcd_Timing0 = 0x1F0A0338;
    lcd_Timing1 = 0x0402093F;
    lcd_Timing2 = 0x00EF7802;
    lcd_Timing3 = 0x00000000;

    lcd_UpBaseFull = (uint32_t)RAM_OFFSET(lcd_Ram);
    lcd_LpBaseFull = 0x00000000;
    lcd_Control    = LCD_BGR16bit;

    lcd_EnableInt       = 0;
    lcd_IntStatus       = 0;
    lcd_IntStatusMasked = 0;
    lcd_IntAcknowledge  = 0;
    lcd_UpBaseCurrFull  = 0;
    lcd_LpBaseCurrFull  = 0;

    lcd_CrsrCtrl            = 0;
    lcd_CrsrConfig          = 0;
    lcd_CrsrPalette0        = 0;
    lcd_CrsrPalette1        = 0;
    lcd_CrsrXY              = 0;
    lcd_CrsrXYFull          = 0;
    lcd_CrsrX               = 0;
    lcd_CrsrY               = 0;
    lcd_CrsrYFull           = 0;
    lcd_CrsrClip            = 0;
    lcd_CrsrClipX           = 0;
    lcd_CrsrClipY           = 0;
    lcd_CrsrEnableInt       = 0;
    lcd_CrsrIntAcknowledge  = 0;
    lcd_CrsrIntStatus       = 0;
    lcd_CrsrIntStatusMasked = 0;

    lcd_BacklightLevel = 0x80; // Unsure what the default is

    calc_day     = 1   ;
    calc_month   = 1   ;
    calc_year    = 2015;
    calc_hours   = 0   ;
    calc_minutes = 0   ;
    calc_seconds = 0   ;

    /* Timers */
        memset(&simulated_ram[0xF20000], 0, 0x38);
        last_timer_update = getNanoTime();
    /* RTC */
        memset(&simulated_ram[0xF30000], 0, 0x38);
}
