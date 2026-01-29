#include "PortCE_Render.h"
#include <PortCE.h>

#include <keypadc.h>
#include <ti/getcsc.h>
#include <ti/getkey.h>
#include <cstdio>
#include <array>
#include <cassert>

#include "nano_time.hpp"
#include "event.hpp"
#include "key_input.hpp"

#include "ti84pceg.hpp"

#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

//------------------------------------------------------------------------------
// Button_Input
//------------------------------------------------------------------------------

enum class Button_Input {
    btn_Sentinel = -1,
    /* F1 - F5 */
    btn_Yequ,
    btn_Window,
    btn_Zoom,
    btn_Trace,
    btn_Graph,
    /* Orthagonal UDLR */
    btn_Up,
    btn_Down,
    btn_Left,
    btn_Right,
    /* 2nd */
    btn_2nd,
    btn_Mode,
    btn_Del,
    /* Alpha */
    btn_Alpha,
    btn_GraphVar,
    btn_Stat,
    /* Row 4 */
    btn_Math,
    btn_Apps,
    btn_Prgm,
    btn_Vars,
    btn_Clear,
    /* Row 5 */
    btn_Recip,
    btn_Sin,
    btn_Cos,
    btn_Tan,
    btn_Power,
    /* Row 6 */
    btn_Square,
    btn_Comma,
    btn_LParen,
    btn_RParen,
    btn_Div,
    /* Row 7 */
    btn_Log,
    btn_7,
    btn_8,
    btn_9,
    btn_Mul,
    /* Row 8 */
    btn_Ln,
    btn_4,
    btn_5,
    btn_6,
    btn_Sub,
    /* Row 9 */
    btn_Sto,
    btn_1,
    btn_2,
    btn_3,
    btn_Add,
    /* Row 10 */
    btn_On,
    btn_0,
    btn_DecPnt,
    btn_Chs,
    btn_Enter,
    /* Count */
    Count,
};

class Button_State {
    nano64_t timestamp_pressed;
    nano64_t timestamp_released;
    bool pressed;
public:
    Button_State() {
        timestamp_pressed = 0;
        timestamp_released = 0;
        pressed = false;
    }
    bool is_pressed() const {
        return pressed;
    }
    bool is_released() const {
        return !pressed;
    }
    nano64_t get_timestamp_pressed() const {
        return timestamp_pressed;
    }
    nano64_t get_timestamp_released() const {
        return timestamp_released;
    }
    void press_key(nano64_t current_time, bool idempotent = true) {
        if (is_pressed() && idempotent) {
            return;
        }
        timestamp_pressed = current_time;
        pressed = true;
    }
    void release_key(nano64_t current_time, bool idempotent = true) {
        if (is_released() && idempotent) {
            return;
        }
        timestamp_released = current_time;
        pressed = false;
    }
    nano64_t duration_pressed(nano64_t current_time) const {
        if (!is_pressed()) {
            return 0;
        }
        return (timestamp_pressed - current_time);
    }
    nano64_t duration_released(nano64_t current_time) const {
        if (!is_released()) {
            return 0;
        }
        return (timestamp_pressed - current_time);
    }
};

static std::array<Button_State, static_cast<size_t>(Button_Input::Count)> button_state;

static const Button_State& get_button_state(Button_Input button) {
    assert(button <= Button_Input::btn_Count && button != Button_Input::btn_Sentinel);
    return button_state[static_cast<size_t>(button)];
}

static void PortCE_press_button(Button_Input button) {
    button_state[static_cast<size_t>(button)].press_key(getNanoTime());
}

static void PortCE_release_button(Button_Input button) {
    button_state[static_cast<size_t>(button)].release_key(getNanoTime());
}

//------------------------------------------------------------------------------
// Conversions
//------------------------------------------------------------------------------

static Button_Input to_Button_Input(Key_Input k) {
    switch (k) {
        case Key_Input::key_Down     : return Button_Input::btn_Down     ;
        case Key_Input::key_Left     : return Button_Input::btn_Left     ;
        case Key_Input::key_Right    : return Button_Input::btn_Right    ;
        case Key_Input::key_Up       : return Button_Input::btn_Up       ;
        case Key_Input::key_Enter    : return Button_Input::btn_Enter    ;
        case Key_Input::key_2nd      : return Button_Input::btn_2nd      ;
        case Key_Input::key_Clear    : return Button_Input::btn_Clear    ;
        case Key_Input::key_Alpha    : return Button_Input::btn_Alpha    ;
        case Key_Input::key_Add      : return Button_Input::btn_Add      ;
        case Key_Input::key_Sub      : return Button_Input::btn_Sub      ;
        case Key_Input::key_Mul      : return Button_Input::btn_Mul      ;
        case Key_Input::key_Div      : return Button_Input::btn_Div      ;
        case Key_Input::key_Graph    : return Button_Input::btn_Graph    ;
        case Key_Input::key_Trace    : return Button_Input::btn_Trace    ;
        case Key_Input::key_Zoom     : return Button_Input::btn_Zoom     ;
        case Key_Input::key_Window   : return Button_Input::btn_Window   ;
        case Key_Input::key_Yequ     : return Button_Input::btn_Yequ     ;
        case Key_Input::key_Mode     : return Button_Input::btn_Mode     ;
        case Key_Input::key_Del      : return Button_Input::btn_Del      ;
        case Key_Input::key_Sto      : return Button_Input::btn_Sto      ;
        case Key_Input::key_Ln       : return Button_Input::btn_Ln       ;
        case Key_Input::key_Log      : return Button_Input::btn_Log      ;
        case Key_Input::key_Square   : return Button_Input::btn_Square   ;
        case Key_Input::key_Recip    : return Button_Input::btn_Recip    ;
        case Key_Input::key_Math     : return Button_Input::btn_Math     ;
        case Key_Input::key_0        : return Button_Input::btn_0        ;
        case Key_Input::key_1        : return Button_Input::btn_1        ;
        case Key_Input::key_4        : return Button_Input::btn_4        ;
        case Key_Input::key_7        : return Button_Input::btn_7        ;
        case Key_Input::key_2        : return Button_Input::btn_2        ;
        case Key_Input::key_5        : return Button_Input::btn_5        ;
        case Key_Input::key_8        : return Button_Input::btn_8        ;
        case Key_Input::key_3        : return Button_Input::btn_3        ;
        case Key_Input::key_6        : return Button_Input::btn_6        ;
        case Key_Input::key_9        : return Button_Input::btn_9        ;
        case Key_Input::key_Comma    : return Button_Input::btn_Comma    ;
        case Key_Input::key_Sin      : return Button_Input::btn_Sin      ;
        case Key_Input::key_Apps     : return Button_Input::btn_Apps     ;
        case Key_Input::key_GraphVar : return Button_Input::btn_GraphVar ;
        case Key_Input::key_DecPnt   : return Button_Input::btn_DecPnt   ;
        case Key_Input::key_LParen   : return Button_Input::btn_LParen   ;
        case Key_Input::key_Cos      : return Button_Input::btn_Cos      ;
        case Key_Input::key_Prgm     : return Button_Input::btn_Prgm     ;
        case Key_Input::key_Stat     : return Button_Input::btn_Stat     ;
        case Key_Input::key_Chs      : return Button_Input::btn_Chs      ;
        case Key_Input::key_RParen   : return Button_Input::btn_RParen   ;
        case Key_Input::key_Tan      : return Button_Input::btn_Tan      ;
        case Key_Input::key_Vars     : return Button_Input::btn_Vars     ;
        case Key_Input::key_Power    : return Button_Input::btn_Power    ;
        default: return Button_Input::btn_Sentinel;
    }
}

static void Key_Input_set_key_down(Key_Input code) {
    Button_Input btn = to_Button_Input(code);
    if (btn == Button_Input::btn_Sentinel) {
        return;
    }
    PortCE_press_button(btn);
}

static void Key_Input_set_key_up(Key_Input code) {
    Button_Input btn = to_Button_Input(code);
    if (btn == Button_Input::btn_Sentinel) {
        return;
    }
    PortCE_release_button(btn);
}

//------------------------------------------------------------------------------
// Key press/release
//------------------------------------------------------------------------------

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
void Key_Input_key_down(Key_Input code) {
    switch (code) {
        case Key_Input::key_UpLeft: {
            Key_Input_set_key_down(Key_Input::key_Up);
            Key_Input_set_key_down(Key_Input::key_Left);
        } break;
        case Key_Input::key_UpRight: {
            Key_Input_set_key_down(Key_Input::key_Up);
            Key_Input_set_key_down(Key_Input::key_Right);
        } break;
        case Key_Input::key_DownLeft: {
            Key_Input_set_key_down(Key_Input::key_Down);
            Key_Input_set_key_down(Key_Input::key_Left);
        } break;
        case Key_Input::key_DownRight: {
            Key_Input_set_key_down(Key_Input::key_Down);
            Key_Input_set_key_down(Key_Input::key_Right);
        } break;
        default: {
            Key_Input_set_key_down(code);
        } break;
    }
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
void Key_Input_key_up(Key_Input code) {
    switch (code) {
        case Key_Input::key_UpLeft: {
            Key_Input_set_key_up(Key_Input::key_Up);
            Key_Input_set_key_up(Key_Input::key_Left);
        } break;
        case Key_Input::key_UpRight: {
            Key_Input_set_key_up(Key_Input::key_Up);
            Key_Input_set_key_up(Key_Input::key_Right);
        } break;
        case Key_Input::key_DownLeft: {
            Key_Input_set_key_up(Key_Input::key_Down);
            Key_Input_set_key_up(Key_Input::key_Left);
        } break;
        case Key_Input::key_DownRight: {
            Key_Input_set_key_up(Key_Input::key_Down);
            Key_Input_set_key_up(Key_Input::key_Right);
        } break;
        default: {
            Key_Input_set_key_up(code);
        } break;
    }
}

//------------------------------------------------------------------------------
// <keypadc.h>
//------------------------------------------------------------------------------

// enum keyBit {
//     kb_Graph = 1    , kb_Trace = 2  , kb_Zoom = 4   , kb_Window = 8 , kb_Yequ = 16  , kb_2nd = 32   , kb_Mode = 64  , kb_Del = 128      ,
//                       kb_Sto = 2    , kb_Ln = 4     , kb_Log = 8    , kb_Square = 16, kb_Recip = 32 , kb_Math = 64  , kb_Alpha = 128    ,
//     kb_0 = 1        , kb_1 = 2      , kb_4 = 4      , kb_7 = 8      , kb_Comma = 16 , kb_Sin = 32   , kb_Apps = 64  , kb_GraphVar = 128 ,
//     kb_DecPnt = 1   , kb_2 = 2      , kb_5 = 4      , kb_8 = 8      , kb_LParen = 16, kb_Cos = 32   , kb_Prgm = 64  , kb_Stat = 128     ,
//     kb_Chs = 1      , kb_3 = 2      , kb_6 = 4      , kb_9 = 8      , kb_RParen = 16, kb_Tan = 32   , kb_Vars = 64  ,
//     kb_Enter = 1    , kb_Add = 2    , kb_Sub = 4    , kb_Mul = 8    , kb_Div = 16   , kb_Power = 32 , kb_Clear = 64 ,
//     kb_Down = 1     , kb_Left = 2   , kb_Right = 4  , kb_Up = 8
// };

#define internal_kb_Data (static_cast<uint16_t*>(RAM_ADDRESS(ti::kbdG1)) - 1)

#define kb_DI_Mode     static_cast<uint8_t*>(RAM_ADDRESS(ti::DI_Mode))
#define kb_DI_Cntrl    static_cast<uint8_t*>(RAM_ADDRESS(ti::DI_Cntrl))
#define kb_DI_Int      static_cast<uint8_t*>(RAM_ADDRESS(ti::DI_Int))
#define kb_DI_IntMask  static_cast<uint8_t*>(RAM_ADDRESS(ti::DI_IntMask))

constexpr int KB_Row_Count = 8;
__attribute__((__unused__)) constexpr int KB_Max_Key_Code = 64;

typedef enum PortCE_Button_Code {
    BTN_None,
    BTN_Graph = 8    , BTN_Trace    , BTN_Zoom  , BTN_Window , BTN_Yequ   , BTN_2nd   , BTN_Mode  , BTN_Del      ,
    /*              */ BTN_Sto = 17 , BTN_Ln    , BTN_Log    , BTN_Square , BTN_Recip , BTN_Math  , BTN_Alpha    ,
    BTN_0            , BTN_1        , BTN_4     , BTN_7      , BTN_Comma  , BTN_Sin   , BTN_Apps  , BTN_GraphVar ,
    BTN_DecPnt       , BTN_2        , BTN_5     , BTN_8      , BTN_LParen , BTN_Cos   , BTN_Prgm  , BTN_Stat     ,
    BTN_Chs          , BTN_3        , BTN_6     , BTN_9      , BTN_RParen , BTN_Tan   , BTN_Vars  ,
    BTN_Enter = 48   , BTN_Add      , BTN_Sub   , BTN_Mul    , BTN_Div    , BTN_Power , BTN_Clear ,
    BTN_Down = 56    , BTN_Left     , BTN_Right , BTN_Up
} PortCE_Button_Code;

static void setKey(uint8_t k) {
    if (k == BTN_None) {
        return;
    }
    assert(k < KB_Max_Key_Code);
    uint8_t bit = static_cast<uint8_t>(1 << (k % 8));
    uint8_t off = k / 8;
    internal_kb_Data[off] |= (bit);
}

static void resetKey(uint8_t k) {
    if (k == BTN_None) {
        return;
    }
    assert(k < KB_Max_Key_Code);
    uint8_t bit = static_cast<uint8_t>(1 << (k % 8));
    uint8_t off = k / 8;
    internal_kb_Data[off] &= ~(bit);
}

static uint8_t to_Button_Code(Button_Input button) {
    switch (button) {
        case Button_Input::btn_Down     : return BTN_Down     ;
        case Button_Input::btn_Left     : return BTN_Left     ;
        case Button_Input::btn_Right    : return BTN_Right    ;
        case Button_Input::btn_Up       : return BTN_Up       ;
        case Button_Input::btn_Enter    : return BTN_Enter    ;
        case Button_Input::btn_2nd      : return BTN_2nd      ;
        case Button_Input::btn_Clear    : return BTN_Clear    ;
        case Button_Input::btn_Alpha    : return BTN_Alpha    ;
        case Button_Input::btn_Add      : return BTN_Add      ;
        case Button_Input::btn_Sub      : return BTN_Sub      ;
        case Button_Input::btn_Mul      : return BTN_Mul      ;
        case Button_Input::btn_Div      : return BTN_Div      ;
        case Button_Input::btn_Graph    : return BTN_Graph    ;
        case Button_Input::btn_Trace    : return BTN_Trace    ;
        case Button_Input::btn_Zoom     : return BTN_Zoom     ;
        case Button_Input::btn_Window   : return BTN_Window   ;
        case Button_Input::btn_Yequ     : return BTN_Yequ     ;
        case Button_Input::btn_Mode     : return BTN_Mode     ;
        case Button_Input::btn_Del      : return BTN_Del      ;
        case Button_Input::btn_Sto      : return BTN_Sto      ;
        case Button_Input::btn_Ln       : return BTN_Ln       ;
        case Button_Input::btn_Log      : return BTN_Log      ;
        case Button_Input::btn_Square   : return BTN_Square   ;
        case Button_Input::btn_Recip    : return BTN_Recip    ;
        case Button_Input::btn_Math     : return BTN_Math     ;
        case Button_Input::btn_0        : return BTN_0        ;
        case Button_Input::btn_1        : return BTN_1        ;
        case Button_Input::btn_4        : return BTN_4        ;
        case Button_Input::btn_7        : return BTN_7        ;
        case Button_Input::btn_2        : return BTN_2        ;
        case Button_Input::btn_5        : return BTN_5        ;
        case Button_Input::btn_8        : return BTN_8        ;
        case Button_Input::btn_3        : return BTN_3        ;
        case Button_Input::btn_6        : return BTN_6        ;
        case Button_Input::btn_9        : return BTN_9        ;
        case Button_Input::btn_Comma    : return BTN_Comma    ;
        case Button_Input::btn_Sin      : return BTN_Sin      ;
        case Button_Input::btn_Apps     : return BTN_Apps     ;
        case Button_Input::btn_GraphVar : return BTN_GraphVar ;
        case Button_Input::btn_DecPnt   : return BTN_DecPnt   ;
        case Button_Input::btn_LParen   : return BTN_LParen   ;
        case Button_Input::btn_Cos      : return BTN_Cos      ;
        case Button_Input::btn_Prgm     : return BTN_Prgm     ;
        case Button_Input::btn_Stat     : return BTN_Stat     ;
        case Button_Input::btn_Chs      : return BTN_Chs      ;
        case Button_Input::btn_RParen   : return BTN_RParen   ;
        case Button_Input::btn_Tan      : return BTN_Tan      ;
        case Button_Input::btn_Vars     : return BTN_Vars     ;
        case Button_Input::btn_Power    : return BTN_Power    ;
        default: return BTN_None;
    }
}

static void keypadc_kb_scan(void) {
    *kb_DI_Mode = 0x02;
    handle_SDL_events();
    for (size_t i = 0; i < button_state.size(); i++) {
        if (button_state[i].is_pressed()) {
            setKey(to_Button_Code(static_cast<Button_Input>(i)));
        }
        if (button_state[i].is_released()) {
            resetKey(to_Button_Code(static_cast<Button_Input>(i)));
        }
    }
    *kb_DI_Mode = 0x00;
}

void kb_Scan(void) {
    keypadc_kb_scan();
}

void kb_Reset(void) {
    // ld (ti.DI_Mode + 0), $000F00
    kb_DI_Mode[0] = 0x00;
    kb_DI_Mode[1] = 0x0F;
    kb_DI_Mode[2] = 0x00;
    // ld (ti.DI_Mode + 3), $08080F
    kb_DI_Mode[3] = 0x0F;
    kb_DI_Mode[4] = 0x08;
    kb_DI_Mode[5] = 0x08;
}

kb_key_t kb_ScanGroup(uint8_t row) {
    if (row >= KB_Row_Count) {
        return 0;
    }
    keypadc_kb_scan();
    return static_cast<kb_key_t>(internal_kb_Data[row]);
}

uint8_t kb_AnyKey(void) {
    keypadc_kb_scan();
    uint8_t ret = 0;
    for (int i = 1; i < KB_Row_Count; i++) {
        ret |= static_cast<uint8_t>(internal_kb_Data[i]);
    }
    return ret;
}

//------------------------------------------------------------------------------
// <ti/getcsc.h>
//------------------------------------------------------------------------------

bool boot_CheckOnPressed(void) {
    return get_button_state(Button_Input::btn_On).is_pressed();
}

static Button_Input internal_CSC_Scan(void) {
    handle_SDL_events();
    for (size_t i = 0; i < button_state.size(); i++) {
        if (button_state[i].is_pressed()) {
            return static_cast<Button_Input>(i);
        }
    }
    return Button_Input::btn_Sentinel;
}

static uint8_t get_KbdKey(Button_Input button) {
    uint8_t code = 0;
    switch (button) {
        case Button_Input::btn_Down     : code = sk_Down     ; break;
        case Button_Input::btn_Left     : code = sk_Left     ; break;
        case Button_Input::btn_Right    : code = sk_Right    ; break;
        case Button_Input::btn_Up       : code = sk_Up       ; break;
        case Button_Input::btn_Enter    : code = sk_Enter    ; break;
        case Button_Input::btn_2nd      : code = sk_2nd      ; break;
        case Button_Input::btn_Clear    : code = sk_Clear    ; break;
        case Button_Input::btn_Alpha    : code = sk_Alpha    ; break;
        case Button_Input::btn_Add      : code = sk_Add      ; break;
        case Button_Input::btn_Sub      : code = sk_Sub      ; break;
        case Button_Input::btn_Mul      : code = sk_Mul      ; break;
        case Button_Input::btn_Div      : code = sk_Div      ; break;
        case Button_Input::btn_Graph    : code = sk_Graph    ; break;
        case Button_Input::btn_Trace    : code = sk_Trace    ; break;
        case Button_Input::btn_Zoom     : code = sk_Zoom     ; break;
        case Button_Input::btn_Window   : code = sk_Window   ; break;
        case Button_Input::btn_Yequ     : code = sk_Yequ     ; break;
        case Button_Input::btn_Mode     : code = sk_Mode     ; break;
        case Button_Input::btn_Del      : code = sk_Del      ; break;
        case Button_Input::btn_Sto      : code = sk_Store    ; break;
        case Button_Input::btn_Ln       : code = sk_Ln       ; break;
        case Button_Input::btn_Log      : code = sk_Log      ; break;
        case Button_Input::btn_Square   : code = sk_Square   ; break;
        case Button_Input::btn_Recip    : code = sk_Recip    ; break;
        case Button_Input::btn_Math     : code = sk_Math     ; break;
        case Button_Input::btn_0        : code = sk_0        ; break;
        case Button_Input::btn_1        : code = sk_1        ; break;
        case Button_Input::btn_4        : code = sk_4        ; break;
        case Button_Input::btn_7        : code = sk_7        ; break;
        case Button_Input::btn_2        : code = sk_2        ; break;
        case Button_Input::btn_5        : code = sk_5        ; break;
        case Button_Input::btn_8        : code = sk_8        ; break;
        case Button_Input::btn_3        : code = sk_3        ; break;
        case Button_Input::btn_6        : code = sk_6        ; break;
        case Button_Input::btn_9        : code = sk_9        ; break;
        case Button_Input::btn_Comma    : code = sk_Comma    ; break;
        case Button_Input::btn_Sin      : code = sk_Sin      ; break;
        case Button_Input::btn_Apps     : code = sk_Apps     ; break;
        case Button_Input::btn_GraphVar : code = sk_GraphVar ; break;
        case Button_Input::btn_DecPnt   : code = sk_DecPnt   ; break;
        case Button_Input::btn_LParen   : code = sk_LParen   ; break;
        case Button_Input::btn_Cos      : code = sk_Cos      ; break;
        case Button_Input::btn_Prgm     : code = sk_Prgm     ; break;
        case Button_Input::btn_Stat     : code = sk_Stat     ; break;
        case Button_Input::btn_Chs      : code = sk_Chs      ; break;
        case Button_Input::btn_RParen   : code = sk_RParen   ; break;
        case Button_Input::btn_Tan      : code = sk_Tan      ; break;
        case Button_Input::btn_Vars     : code = sk_Vars     ; break;
        case Button_Input::btn_Power    : code = sk_Power    ; break;
        default: code = 0;
    }
    os_KbdKey = code;
    return code;
}

uint8_t os_GetCSC(void) {
    static nano64_t prev_time = 0;
    constexpr double framerate_delay = 60.0;
#ifdef __EMSCRIPTEN__
    const double frame_ms = 1000.0 / framerate_delay;
    double current_ms = emscripten_get_now();
    if (current_ms - (double)prev_time < frame_ms) {
        emscripten_sleep(0);
        return 0;
    }
    prev_time = (nano64_t)current_ms;
    PortCE_update_registers();
#else
    nano64_t current_time;
    do  {
        current_time = getNanoTime();
        // or some other busy wait function idk
        PortCE_update_registers();
    } while (current_time - prev_time < FRAMERATE_TO_NANO(framerate_delay));
    prev_time = current_time;
#endif

    Button_Input key = internal_CSC_Scan();
    PortCE_new_frame();

    return get_KbdKey(key);
}

//------------------------------------------------------------------------------
// <ti/getcsc.h>
//------------------------------------------------------------------------------

static uint8_t internal_KdbGetKy(Button_Input key) {
    switch (key) {
        case Button_Input::btn_Down    : return k_Down    ;
        case Button_Input::btn_Left    : return k_Left    ;
        case Button_Input::btn_Right   : return k_Right   ;
        case Button_Input::btn_Up      : return k_Up      ;
        case Button_Input::btn_Enter   : return k_Enter   ;
        // case Button_Input::btn_2nd     : return k_2nd     ;
        case Button_Input::btn_Clear   : return k_Clear   ;
        // case Button_Input::btn_Alpha   : return k_Alpha   ;
        case Button_Input::btn_Add     : return k_Add     ;
        case Button_Input::btn_Sub     : return k_Sub     ;
        case Button_Input::btn_Mul     : return k_Mul     ;
        case Button_Input::btn_Div     : return k_Div     ;
        case Button_Input::btn_Graph   : return k_Graph   ;
        case Button_Input::btn_Trace   : return k_Trace   ;
        case Button_Input::btn_Zoom    : return k_Zoom    ;
        case Button_Input::btn_Window  : return k_Window  ;
        case Button_Input::btn_Yequ    : return k_Yequ    ;
        case Button_Input::btn_Mode    : return k_Mode    ;
        case Button_Input::btn_Del     : return k_Del     ;
        case Button_Input::btn_Sto     : return k_Store   ;
        case Button_Input::btn_Ln      : return k_Ln      ;
        case Button_Input::btn_Log     : return k_Log     ;
        case Button_Input::btn_Square  : return k_Square  ;
        // case Button_Input::btn_Recip   : return k_Recip   ;
        case Button_Input::btn_Math    : return k_Math    ;
        case Button_Input::btn_0       : return k_0       ;
        case Button_Input::btn_1       : return k_1       ;
        case Button_Input::btn_4       : return k_4       ;
        case Button_Input::btn_7       : return k_7       ;
        case Button_Input::btn_2       : return k_2       ;
        case Button_Input::btn_5       : return k_5       ;
        case Button_Input::btn_8       : return k_8       ;
        case Button_Input::btn_3       : return k_3       ;
        case Button_Input::btn_6       : return k_6       ;
        case Button_Input::btn_9       : return k_9       ;
        case Button_Input::btn_Comma   : return k_Comma   ;
        case Button_Input::btn_Sin     : return k_Sin     ;
        // case Button_Input::btn_Apps    : return k_Apps    ;
        // case Button_Input::btn_GraphVar: return k_GraphVar;
        case Button_Input::btn_DecPnt  : return k_DecPnt  ;
        case Button_Input::btn_LParen  : return k_LParen  ;
        case Button_Input::btn_Cos     : return k_Cos     ;
        case Button_Input::btn_Prgm    : return k_Prgm    ;
        case Button_Input::btn_Stat    : return k_Stat    ;
        case Button_Input::btn_Chs     : return k_Chs     ;
        case Button_Input::btn_RParen  : return k_RParen  ;
        case Button_Input::btn_Tan     : return k_Tan     ;
        case Button_Input::btn_Vars    : return k_Vars    ;
        // case Button_Input::btn_Power   : return k_Power   ;
        default: return 0;
    }
}

uint16_t os_GetKey(void) {
    Button_Input key = Button_Input::btn_Sentinel;

    while (key == Button_Input::btn_Sentinel) {
        key = internal_CSC_Scan();
        PortCE_new_frame();
#ifdef __EMSCRIPTEN__
        if (key == Button_Input::btn_Sentinel) {
            emscripten_sleep(0);
        }
#endif
    }

    os_KbdGetKy = internal_KdbGetKy(key);
    os_KeyExtend = 0;

    return static_cast<uint16_t>(os_KbdGetKy | (os_KeyExtend << 8));
}

//------------------------------------------------------------------------------
