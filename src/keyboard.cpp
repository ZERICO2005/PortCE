
#include <PortCE_Keybinds.h>
#include "PortCE_Render.h"
#include "PortCE_Common.h"

#include <cstdlib>
#include <keypadc.h>
#include <ti/getkey.h>
#include <ti/getcsc.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

struct bound {
    uint8_t x0;
    uint8_t y0;
    uint8_t x1;
    uint8_t y1;
};

__attribute__((__unused__))
static struct bound expandedPos[56] = {
    {1,1,4,2},{6,1,4,2},{11,1,4,2},{16,1,4,2},{21,1,4,2}, //F1-F5
    {19,4,3,3},{19,10,3,3},{16,7,3,3},{22,7,3,3},//Orthagonal UDLR
    {22,4,3,3},{16,10,3,3},{16,4,3,3},{22,10,3,3},//Diagonal Shift Clockwise
    {1,5,4,3},{6,5,4,3},{11,5,4,3}, //2nd
    {1,9,4,3},{6,9,4,3},{11,9,4,3}, //Alpha
    {1,14,4,2},{6,14,4,2},{11,14,4,2},{16,14,4,2},{21,14,4,2},
    {1,17,4,2},{6,17,4,2},{11,17,4,2},{16,17,4,2},{21,17,4,2},
    {1,20,4,2},{6,20,4,2},{11,20,4,2},{16,20,4,2},{21,20,4,2},

    {1,23,4,2},{6,24,4,2},{11,24,4,2},{16,24,4,2},{21,23,4,2},
    {1,26,4,2},{6,27,4,2},{11,27,4,2},{16,27,4,2},{21,26,4,2},
    {1,29,4,2},{6,30,4,2},{11,30,4,2},{16,30,4,2},{21,29,4,2},
    {1,32,4,2},{6,33,4,2},{11,33,4,2},{16,33,4,2},{21,32,4,2},
    {1,35,4,2},{1,36,4,1} //On
};

/* Replacement Libary Functions */

static const uint8_t* KEYS;
static int KEYCOUNT;

// enum keyBit {
//     kb_Graph = 1    , kb_Trace = 2  , kb_Zoom = 4   , kb_Window = 8 , kb_Yequ = 16  , kb_2nd = 32   , kb_Mode = 64  , kb_Del = 128      ,
//                       kb_Sto = 2    , kb_Ln = 4     , kb_Log = 8    , kb_Square = 16, kb_Recip = 32 , kb_Math = 64  , kb_Alpha = 128    ,
//     kb_0 = 1        , kb_1 = 2      , kb_4 = 4      , kb_7 = 8      , kb_Comma = 16 , kb_Sin = 32   , kb_Apps = 64  , kb_GraphVar = 128 ,
//     kb_DecPnt = 1   , kb_2 = 2      , kb_5 = 4      , kb_8 = 8      , kb_LParen = 16, kb_Cos = 32   , kb_Prgm = 64  , kb_Stat = 128     ,
//     kb_Chs = 1      , kb_3 = 2      , kb_6 = 4      , kb_9 = 8      , kb_RParen = 16, kb_Tan = 32   , kb_Vars = 64  ,
//     kb_Enter = 1    , kb_Add = 2    , kb_Sub = 4    , kb_Mul = 8    , kb_Div = 16   , kb_Power = 32 , kb_Clear = 64 ,
//     kb_Down = 1     , kb_Left = 2   , kb_Right = 4  , kb_Up = 8
// };

#define internal_kb_Data(k) (((uint16_t*)RAM_ADDRESS(0xF50010))[k])

static void setKey(uint8_t k) {
    if (k >= 64) { //Out of Bounds
        return;
    }
    uint8_t bit = static_cast<uint8_t>(1 << (k % 8));
    uint8_t off = k / 8;
    internal_kb_Data(off + 1) |= (bit); //+1 otherwise down becomes enter
}
static void resetKey(uint8_t k) {
    if (k >= 64) { //Out of Bounds
        return;
    }
    uint8_t bit = static_cast<uint8_t>(1 << (k % 8));
    uint8_t off = k / 8;
    internal_kb_Data(off + 1) &= ~(bit);
}

/* Replacement Libary Functions */

// Allows for the same key to be binded to more than one thing

bool boot_CheckOnPressed(void) {
    return false; // No way to check for the ON key currently
}

static uint8_t internal_kb_Scan(void) {
    static uint8_t tempKey[56];
    SDL_Event event;
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
        terminateLCDcontroller();
        exit(0);
    }
    KEYS = SDL_GetKeyboardState(&KEYCOUNT);
    uint32_t length = sizeof(PortCE_Keybind_Selection)/sizeof(PortCE_Keybind_Selection[0]);
    for (uint32_t i = 0; i < length; i++) {
        if (KEYS[PortCE_Keybind_Selection[i].SDL] == 1) {
            //printf("%s\n",SDL_GetKeyName(PortCE_Keybind_Selection[i].SDL));
            //printf("%d %d,",q,PortCE_Keybind_Selection[i].CE); fflush(stdout);
            setKey(PortCE_Keybind_Selection[i].CE);
            tempKey[PortCE_Keybind_Selection[i].CE] = 1;
        }
    }
    uint8_t pressed_key_count = 0;
    for (uint8_t i = 0; i < 56; i++) {
        if (tempKey[i] == 0) {
            resetKey(i);
        }
        if (tempKey[i] == 1) {
            pressed_key_count++;
        }
        tempKey[i] = 0;
    }
    return pressed_key_count;
}

void kb_Scan(void) {
    internal_kb_Scan();
}

kb_key_t kb_ScanGroup(uint8_t row) {
    internal_kb_Scan();
    return static_cast<kb_key_t>(internal_kb_Data(row));
}

uint8_t kb_AnyKey(void) {
    return internal_kb_Scan();
}

void kb_Reset(void) {
    for (uint8_t i = 0; i < 7; i++) {
        internal_kb_Data(i) = 0;
    }
}

static uint8_t internal_CSC_Scan(void) {
    SDL_Event event;
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
        terminateLCDcontroller();
        exit(0);
    }
    KEYS = SDL_GetKeyboardState(&KEYCOUNT);
    uint32_t length = sizeof(PortCE_Keybind_Selection)/sizeof(PortCE_Keybind_Selection[0]);
    for (uint32_t i = 0; i < length; i++) {
        if (KEYS[PortCE_Keybind_Selection[i].SDL] == 1) {
            return PortCE_Keybind_Selection[i].CE;
        }
    }
    return 0;
}

uint8_t os_GetCSC(void) {
    static nano64_t prev_time = 0;
    nano64_t current_time;
    do  {
        current_time = getNanoTime();
        // or some other busy wait function idk
        PortCE_update_registers();
    } while (current_time - prev_time < FRAMERATE_TO_NANO(60.0));
    prev_time = current_time;

    uint8_t key = internal_CSC_Scan();
    PortCE_new_frame();

    switch (key) {
        case KB_Down    : return sk_Down    ;
        case KB_Left    : return sk_Left    ;
        case KB_Right   : return sk_Right   ;
        case KB_Up      : return sk_Up      ;
        case KB_Enter   : return sk_Enter   ;
        case KB_2nd     : return sk_2nd     ;
        case KB_Clear   : return sk_Clear   ;
        case KB_Alpha   : return sk_Alpha   ;
        case KB_Add     : return sk_Add     ;
        case KB_Sub     : return sk_Sub     ;
        case KB_Mul     : return sk_Mul     ;
        case KB_Div     : return sk_Div     ;
        case KB_Graph   : return sk_Graph   ;
        case KB_Trace   : return sk_Trace   ;
        case KB_Zoom    : return sk_Zoom    ;
        case KB_Window  : return sk_Window  ;
        case KB_Yequ    : return sk_Yequ    ;
        case KB_Mode    : return sk_Mode    ;
        case KB_Del     : return sk_Del     ;
        case KB_Sto     : return sk_Store   ;
        case KB_Ln      : return sk_Ln      ;
        case KB_Log     : return sk_Log     ;
        case KB_Square  : return sk_Square  ;
        case KB_Recip   : return sk_Recip   ;
        case KB_Math    : return sk_Math    ;
        case KB_0       : return sk_0       ;
        case KB_1       : return sk_1       ;
        case KB_4       : return sk_4       ;
        case KB_7       : return sk_7       ;
        case KB_2       : return sk_2       ;
        case KB_5       : return sk_5       ;
        case KB_8       : return sk_8       ;
        case KB_3       : return sk_3       ;
        case KB_6       : return sk_6       ;
        case KB_9       : return sk_9       ;
        case KB_Comma   : return sk_Comma   ;
        case KB_Sin     : return sk_Sin     ;
        case KB_Apps    : return sk_Apps    ;
        case KB_GraphVar: return sk_GraphVar;
        case KB_DecPnt  : return sk_DecPnt  ;
        case KB_LParen  : return sk_LParen  ;
        case KB_Cos     : return sk_Cos     ;
        case KB_Prgm    : return sk_Prgm    ;
        case KB_Stat    : return sk_Stat    ;
        case KB_Chs     : return sk_Chs     ;
        case KB_RParen  : return sk_RParen  ;
        case KB_Tan     : return sk_Tan     ;
        case KB_Vars    : return sk_Vars    ;
        case KB_Power   : return sk_Power   ;
        case KB_None:
        default: return 0;
    }
}

static uint8_t internal_KdbGetKy(uint8_t key) {
    switch (key) {
        case KB_Down    : return k_Down    ;
        case KB_Left    : return k_Left    ;
        case KB_Right   : return k_Right   ;
        case KB_Up      : return k_Up      ;
        case KB_Enter   : return k_Enter   ;
        // case KB_2nd     : return k_2nd     ;
        case KB_Clear   : return k_Clear   ;
        // case KB_Alpha   : return k_Alpha   ;
        case KB_Add     : return k_Add     ;
        case KB_Sub     : return k_Sub     ;
        case KB_Mul     : return k_Mul     ;
        case KB_Div     : return k_Div     ;
        case KB_Graph   : return k_Graph   ;
        case KB_Trace   : return k_Trace   ;
        case KB_Zoom    : return k_Zoom    ;
        case KB_Window  : return k_Window  ;
        case KB_Yequ    : return k_Yequ    ;
        case KB_Mode    : return k_Mode    ;
        case KB_Del     : return k_Del     ;
        case KB_Sto     : return k_Store   ;
        case KB_Ln      : return k_Ln      ;
        case KB_Log     : return k_Log     ;
        case KB_Square  : return k_Square  ;
        // case KB_Recip   : return k_Recip   ;
        case KB_Math    : return k_Math    ;
        case KB_0       : return k_0       ;
        case KB_1       : return k_1       ;
        case KB_4       : return k_4       ;
        case KB_7       : return k_7       ;
        case KB_2       : return k_2       ;
        case KB_5       : return k_5       ;
        case KB_8       : return k_8       ;
        case KB_3       : return k_3       ;
        case KB_6       : return k_6       ;
        case KB_9       : return k_9       ;
        case KB_Comma   : return k_Comma   ;
        case KB_Sin     : return k_Sin     ;
        // case KB_Apps    : return k_Apps    ;
        // case KB_GraphVar: return k_GraphVar;
        case KB_DecPnt  : return k_DecPnt  ;
        case KB_LParen  : return k_LParen  ;
        case KB_Cos     : return k_Cos     ;
        case KB_Prgm    : return k_Prgm    ;
        case KB_Stat    : return k_Stat    ;
        case KB_Chs     : return k_Chs     ;
        case KB_RParen  : return k_RParen  ;
        case KB_Tan     : return k_Tan     ;
        case KB_Vars    : return k_Vars    ;
        // case KB_Power   : return k_Power   ;
        default: return 0;
    }
}

uint16_t os_GetKey(void) {
    uint8_t key = KB_None;

    while (key == KB_None) {
        key = internal_CSC_Scan();
        PortCE_new_frame();
    }

    os_KbdGetKy = internal_KdbGetKy(key);

    return static_cast<uint16_t>(os_KbdGetKy | (os_KeyExtend << 8));
}

uint8_t os_GetCSC(void);


uint16_t os_GetKey(void);

/**
 * Scans the keyboard to update data values.
 * @note Disables interrupts.
 */
void kb_Scan(void);

/**
 * Scans the given keyboard row and returns the row value.
 * @param[in] row Row to scan.
 * @note Disables interrupts.
 */
kb_key_t kb_ScanGroup(uint8_t row);

/**
 * Scans the keyboard quickly to tell if any key was pressed.
 * @note Disables interrupts.
 */
uint8_t kb_AnyKey(void);

/**
 * Resets the keyboard before returning to the OS.
 * @note Only use if the keyboard timers or number of rows have been modified.
 */
void kb_Reset(void);
