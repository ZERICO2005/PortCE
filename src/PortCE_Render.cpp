/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE_Common.h"
#include "PortCE_Render.h"
#include "PortCE_internal.h"
#include "PortCE.h"

#include "PortCE_Extra.h"
#include <sys/lcd.h>
#include <keypadc.h>
#include <ti/getcsc.h>
#include <ti/getkey.h>
#include <PortCE_Keybinds.h>
#include <lcddrvce.h>
#include "PortCE_SPI.h"
#include <stdio.h>

#include "frame_manipulation.hpp"
#include "key_input.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "key_input.hpp"

#include <pthread.h>
#include <sched.h>

bool PortCE_invert_colors = false;
bool PortCE_color_idle_mode = false;

static bool PortCE_SDL_initialized = false;
static SDL_ScaleMode PortCE_scale_mode = SDL_ScaleModeNearest;

static int RESX_MINIMUM = LCD_RESX;
static int RESY_MINIMUM = LCD_RESY;

/* Modern Code */

struct BufferBox {
    uint32_t* vram;
    int resX;
    int resY;
    size_t pitch;
}; typedef struct BufferBox BufferBox;

static BufferBox Master;

static SDL_Event event;
static SDL_Window* window;
static SDL_Texture* texture;
static SDL_Renderer *renderer;

uint32_t PortCE_get_mouse_state(int32_t* posX, int32_t* posY) {
    if (PortCE_SDL_initialized == false) {
        return 0;
    }
    int32_t x, y;
    uint32_t state = SDL_GetMouseState(&x,&y);
    int32_t window_ResX, window_ResY;
    SDL_GetWindowSize(window, &window_ResX, &window_ResY);

    int32_t offsetX = 0;
    int32_t offsetY = 0;
    int32_t image_ResX = window_ResX;
    int32_t image_ResY = window_ResY;
    {
        const double LCD_AspectRatio = (double)LCD_RESX / (double)LCD_RESY;
        const double LCD_AspectRatio_Inverse = (double)LCD_RESY / (double)LCD_RESX;
        const double window_AspectRatio = (double)window_ResX / (double)window_ResY;

        if (window_AspectRatio > LCD_AspectRatio) {
            image_ResX = (int32_t)((double)window_ResY * LCD_AspectRatio);
            offsetX = (window_ResX - image_ResX) / 2;
        } else if (window_AspectRatio < LCD_AspectRatio) {
            image_ResY = (int32_t)((double)window_ResX * LCD_AspectRatio_Inverse);
            offsetY = (window_ResY - image_ResY) / 2;
        }
    }

    x = ((x - offsetX) * LCD_RESX) / (image_ResX);
    y = ((y - offsetY) * LCD_RESY) / (image_ResY);

    if (posX != nullptr) { *posX = x; }
    if (posY != nullptr) { *posY = y; }
    return state;
}

/* Pointers */

SDL_Event* grab_SDL_event();
SDL_Event* grab_SDL_event() {
    return &event;
}

static uint8_t videoCopy[153600];
static uint16_t paletteRAM[256];

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
static uint8_t PortCE_web_keys[56];

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

static void Key_Input_set_key_down(Key_Input code) {
    const PortCE_Keycode ce = to_PortCE_Keycode(code);
    if (ce == KB_None) {
        return;
    }
    const uint8_t ceKey = static_cast<uint8_t>(ce);
    if (ceKey < 56) {
        PortCE_web_keys[ceKey] = 1;
        setKey(ceKey);
    }
}

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

static void Key_Input_set_key_up(Key_Input code) {
    const PortCE_Keycode ce = to_PortCE_Keycode(code);
    if (ce == KB_None) {
        return;
    }
    const uint8_t ceKey = static_cast<uint8_t>(ce);
    if (ceKey < 56) {
        PortCE_web_keys[ceKey] = 0;
        resetKey(ceKey);
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

/* Replacement Libary Functions */

 //Allows for the same key to be binded to more than one thing

static uint8_t internal_kb_Scan(void) {
    static uint8_t tempKey[56];
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
    for (uint8_t i = 0; i < 56; i++) {
        if (PortCE_web_keys[i] == 1) {
            setKey(i);
            tempKey[i] = 1;
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

bool boot_CheckOnPressed(void) {
    return false; // No way to check for the ON key currently
}

static uint8_t internal_CSC_Scan(void) {
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
    for (uint8_t i = 0; i < 56; i++) {
        if (PortCE_web_keys[i] == 1) {
            return i;
        }
    }
    return 0;
}

uint8_t os_GetCSC(void) {
    static nano64_t prev_time = 0;
#ifdef __EMSCRIPTEN__
    const double frame_ms = 1000.0 / 60.0;
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
    } while (current_time - prev_time < FRAMERATE_TO_NANO(60.0));
    prev_time = current_time;
#endif

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
#ifdef __EMSCRIPTEN__
        if (key == KB_None) {
            emscripten_sleep(0);
        }
#endif
    }

    os_KbdGetKy = internal_KdbGetKy(key);

    return static_cast<uint16_t>(os_KbdGetKy | (os_KeyExtend << 8));
}

/**
 * @brief converts the cursor from a 2bit to an 8bit image
 *
 * @param dst the unpacked 8bit cursor image
 * @param src the packed 2bit cursor image
 */
static void unpack_cursor(uint8_t* dst, const uint8_t* src, __attribute__((unused)) bool transpose) {
    // const uint8_t pixelsPerByte = 4;
    // const uint8_t bitsPerPixel = 2;
    for (size_t i = 0; i < lcd_CrsrImageLen64; i++) {
        uint8_t pix = src[i];
        *dst = (pix >> 6) & 0b11; dst++;
        *dst = (pix >> 4) & 0b11; dst++;
        *dst = (pix >> 2) & 0b11; dst++;
        *dst = (pix >> 0) & 0b11; dst++;
    }
}

// Will be optimized later on
static void renderCursor(uint32_t* data) {

    if (data == nullptr) { return; }
    // srand(1234);
    // for (size_t i = 0; i < 1024; i++) {
    //     lcd_CrsrImage[i] = rand() % 256;
    // }
    const bool cursorEnabled = (lcd_CrsrCtrl & 0x1) ? true : false;
    // printf("\nCursor: %s", cursorEnabled ? "true" : "false");
    // printf("\n\tPos: %hu,%hhu Clip: %hhu,%hhu %u", lcd_CrsrX, lcd_CrsrY, lcd_CrsrClipX, lcd_CrsrClipY, lcd_CrsrClip);
    if (cursorEnabled == false) { return; }
    const bool fullCursor = (lcd_CrsrConfig & 0x1) ? true : false;

    const uint8_t cursorDim = fullCursor ? 64 : 32;

    bool use_columnMajorCursor = PortCE_query_column_major() && false;

    const uint16_t cursor_PosX  = use_columnMajorCursor ? lcd_CrsrY     : lcd_CrsrX    ;
    const uint16_t cursor_PosY  = use_columnMajorCursor ? lcd_CrsrX     : lcd_CrsrY    ;
    const uint8_t  cursor_ClipX = use_columnMajorCursor ? lcd_CrsrClipY : lcd_CrsrClipX;
    const uint8_t  cursor_ClipY = use_columnMajorCursor ? lcd_CrsrClipX : lcd_CrsrClipY;

    uint8_t cursor_image[64 * 64];
    unpack_cursor(cursor_image, lcd_CrsrImage, use_columnMajorCursor);

    if (cursor_ClipX >= cursorDim || cursor_ClipY >= cursorDim) {
        // printf("\nError: Everything is clipped");
        return; // Everything is clipped
    }
    const int LCD_RESV = PortCE_query_column_major() ? LCD_RESX : LCD_RESY;
    if (cursor_PosX >= LCD_RESV || cursor_PosY >= LCD_RESY) {
        // printf("\nError: Out of bounds");
        return; // Out of bounds
    }
    const uint16_t limitX = (cursor_PosX + cursorDim > LCD_RESV) ? static_cast<uint8_t>(( cursorDim - ((cursor_PosX + cursorDim) - LCD_RESV) )) : cursorDim;
    const uint16_t limitY = (cursor_PosY + cursorDim > LCD_RESY) ? static_cast<uint8_t>(( cursorDim - ((cursor_PosY + cursorDim) - LCD_RESY) )) : cursorDim;

    uint32_t color_palette0 = bgr24_to_abgr8888(lcd_CrsrPalette0);
    uint32_t color_palette1 = bgr24_to_abgr8888(lcd_CrsrPalette1);

    uint32_t invert_color_mask = 0x00FFFFFF;
    if (PortCE_color_idle_mode) {
        invert_color_mask = 0x00808080;
        color_palette0 &= 0xFF808080;
        color_palette1 &= 0xFF808080;
    }

    const size_t cursor_Offset = fullCursor ? 0 : ((size_t)((lcd_CrsrCtrl >> 4) & 0b11) * 0x100);
    for (uint16_t y = cursor_ClipY; y < limitY; y++) {
        for (uint16_t x = cursor_ClipX; x < limitX; x++) {
            const size_t data_Index = ((size_t)((uint16_t)cursor_PosY + (uint16_t)y) * LCD_RESX) + (size_t)((uint16_t)cursor_PosX + (uint16_t)x);
            const size_t cursor_Index = ((size_t)y * (size_t)cursorDim) + (size_t)x + cursor_Offset;
            const uint8_t pixel = cursor_image[cursor_Index];
            switch (pixel) {
                case lcd_CrsrPixelPalette0:
                    data[data_Index] = color_palette0;
                    break;
                case lcd_CrsrPixelPalette1:
                    data[data_Index] = color_palette1;
                    break;
                case lcd_CrsrPixelTransparent:
                    /* Transparent */
                    break;
                case lcd_CrsrPixelInvert:
                    /* Invert Colors */
                    data[data_Index] = data[data_Index] ^ invert_color_mask;
                    break;
            }
        }
    }
}

#ifdef Debug_Print_LCD_Registers
    void internal_print_LCD_registers(void) {
        #define PrintVar(label, value) \
            printf("\t%12s: %" PRId32 "\n", (label), (int32_t)(value));

        #define PrintHex(label, value) \
            printf("\t%12s: 0x%06" PRIX32 "\n", (label), (int32_t)(value));

        printf("LCD_registers:\n");
        PrintHex("lcd_Control", lcd_Control);
        PrintHex("lcd_UpBase", lcd_UpBase);
        PrintHex("lcd_LpBase", lcd_LpBase);
        #undef PrintHex
        #undef PrintVar
    }
#endif

/**
 * @brief Writes the contents of the ti84ce screen to a buffer
 *
 * @param data buffer to write a LCD_RESX * LCD_RESY image to
 */
void copyFrame(uint32_t* data);
void copyFrame(uint32_t* data) {
    #ifdef Debug_Print_LCD_Registers
        internal_print_LCD_registers();
    #endif

    int width = LCD_RESX;
    int height = LCD_RESY;

    uint16_t lcd_colorMode = (lcd_VideoMode & LCD_MASK_BBP);
    Color_Mode color_mode;
    switch (lcd_colorMode) {
        case LCD_MASK_INDEXED1 : color_mode = Color_Mode::Indexed_1 ; break;
        case LCD_MASK_INDEXED2 : color_mode = Color_Mode::Indexed_2 ; break;
        case LCD_MASK_INDEXED4 : color_mode = Color_Mode::Indexed_4 ; break;
        case LCD_MASK_INDEXED8 : color_mode = Color_Mode::Indexed_8 ; break;
        case LCD_MASK_COLOR1555: color_mode = Color_Mode::Color_1555; break;
        case LCD_MASK_COLOR565 : color_mode = Color_Mode::Color_565 ; break;
        case LCD_MASK_COLOR444 : color_mode = Color_Mode::Color_444 ; break;
        default: color_mode = Color_Mode::Color_555; break;
    };
    size_t pixel_count = (size_t)(width * height);
    size_t copyAmount = (pixel_count * bits_per_pixel(color_mode)) / 8;
    memcpy(videoCopy, ((uint8_t*)&simulated_ram[(0xD00000 | (lcd_UpBase & (0xFFFF << 3)))]), copyAmount);
    memcpy(paletteRAM, lcd_Palette, 256 * sizeof(uint16_t));
    bool bgr_mode = (lcd_VideoMode & LCD_MASK_BGR);
    bool column_major = PortCE_query_column_major();
    Frame_Manipulation frame {
        .src = videoCopy,
        .palette = paletteRAM,
        .dst = data,
        .transpose = column_major,
        .bgr = bgr_mode,
        .invert_colors = PortCE_invert_colors,
        .idle_mode = PortCE_color_idle_mode,
        .color_mode = color_mode,
        .width = width,
        .height = height,
    };

    frame_copy(frame);

    renderCursor(data);
}

/*
**  ===========
**  Modern Code
**  ===========
*/

void PortCE_initialize_sound(void);
void PortCE_terminate_sound(void);

int terminateLCDcontroller() {
    PortCE_terminate_sound();
    PortCE_SDL_initialized = false;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    FREE(Master.vram);
    return EXIT_SUCCESS;
}

void initLCDcontroller(const char* window_title, const PortCE_Config* config) {

    uint8_t init_scale = (config == nullptr) ? 2 : config->window_scale;
    if (init_scale < 1) {
        init_scale = 1;
    } else if (init_scale > 8) {
        init_scale = 8;
    }

    PortCE_scale_mode = (config != nullptr && config->linear_interpolation) ? SDL_ScaleModeLinear : SDL_ScaleModeNearest;

    Master.resX = LCD_RESX;
    Master.resY = LCD_RESY;
    Master.pitch = static_cast<size_t>(Master.resX) * VIDEO_CHANNELS;
    Master.vram = (uint32_t*)calloc((size_t)Master.resY * Master.pitch, sizeof(uint8_t));
    if (Master.vram == nullptr) {
        fprintf(stderr, "Error: Failed to calloc Master.vram\n");
        return;
    }
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        window_title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        LCD_RESX * init_scale, LCD_RESY * init_scale,
        SDL_WINDOW_RESIZABLE
    );

    SDL_SetWindowMinimumSize(window, RESX_MINIMUM, RESY_MINIMUM);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
        LCD_RESX * init_scale, LCD_RESY * init_scale
    );
    SDL_SetTextureScaleMode(texture, PortCE_scale_mode);

    PortCE_initialize_sound();

    PortCE_SDL_initialized = true;
}

void PortCE_set_window_title(const char *window_title) {
    SDL_SetWindowTitle(window, window_title);
}


// Returns True if the window was resized. Optional: Returns new window size.
static bool resizeWindow(int resX, int resY, int* resizeX, int* resizeY) {
    static int resX_prev = 0, resY_prev = 0;
    if (resX_prev == resX && resY_prev == resY) {
        return false;
    }
    resX = resX < RESX_MINIMUM ? RESX_MINIMUM : resX;
    resY = resY < RESY_MINIMUM ? RESY_MINIMUM : resY;

    SDL_SetWindowSize(window,resX,resY);

    SDL_RenderSetLogicalSize(renderer, resX, resY);

    if (resizeX != nullptr) { *resizeX = resX; }
    if (resizeY != nullptr) { *resizeY = resY; }

    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, (int)Master.resX, (int)Master.resY);
    SDL_SetTextureScaleMode(texture, PortCE_scale_mode);
    if (texture == nullptr) {
        fprintf(stderr, "Error: SDL_CreateTexture failed while resizing window\n");
        terminateLCDcontroller();
        return true;
    }

    memset(Master.vram, 0, Master.pitch * static_cast<size_t>(Master.resY));

    return true;
}

// Returns True if the window was resized. Optional: Returns new window size.
static bool windowResizingCode(int* resX, int* resY) {
    int x = 0, y = 0;
    SDL_GetWindowSize(window, &x, &y);
    return resizeWindow(x, y, resX, resY);
}

// #define PORTCE_DEBUG_ALWAYS_PACE_FRAME

static void pace_frame(nano64_t pace_time) {
    const nano64_t yield_threshold = SECONDS_TO_NANO(1.0e-4);

    static nano64_t last_frame_time = 0;
    nano64_t current_time = getNanoTime();

#if 1
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = static_cast<decltype(ts.tv_nsec)>(pace_time - (current_time - last_frame_time));

    if (ts.tv_nsec >= yield_threshold) {
        nanosleep(&ts, nullptr);
    }

#else
    while (current_time - pace_time + yield_threshold < last_frame_time) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            terminateLCDcontroller();
            exit(0);
        }
        sched_yield();
        current_time = getNanoTime();
    }
#endif

    while (current_time - pace_time < last_frame_time) {
        current_time = getNanoTime();
    }
    // printf(" | Sleep: %.3lfms\n", NANO_TO_SECONDS(current_time - sleep_time) * 1.0e3);
    last_frame_time = current_time;
}

void PortCE_new_frame(void) {
    PortCE_update_registers();
    if (PortCE_SDL_initialized == false) {
        return;
    }
    // nano64_t startTime = getNanoTime();

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
        terminateLCDcontroller();
        exit(0);
    }
    windowResizingCode(nullptr,nullptr);
    copyFrame(Master.vram);

    SDL_UpdateTexture(texture, nullptr, Master.vram, (int)Master.pitch);
    {
        // SDL_Rect srcRect = {0,0,(int)Master.resX,(int)Master.resY};
        int window_ResX, window_ResY;
        SDL_GetWindowSize(window, &window_ResX, &window_ResY);
        SDL_Rect dstRect = {0,0,window_ResX,window_ResY};

        const double LCD_AspectRatio = (double)LCD_RESX / (double)LCD_RESY;
        const double LCD_AspectRatio_Inverse = (double)LCD_RESY / (double)LCD_RESX;
        const double window_AspectRatio = (double)window_ResX / (double)window_ResY;

        if (window_AspectRatio > LCD_AspectRatio) {
            int image_ResX = (int)((double)window_ResY * LCD_AspectRatio);
            dstRect = (SDL_Rect){
                (window_ResX - image_ResX) / 2, 0,
                image_ResX, window_ResY
            };
        } else if (window_AspectRatio < LCD_AspectRatio) {
            int image_ResY = (int)((double)window_ResX * LCD_AspectRatio_Inverse);
            dstRect = (SDL_Rect){
                0, (window_ResY - image_ResY) / 2,
                window_ResX, image_ResY
            };
        }

        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    }
    SDL_RenderPresent(renderer);

    // nano64_t endTime = getNanoTime();
    // printf("\nTime: %.3lfms | %.3lffps %lld", NANO_TO_SECONDS(endTime - startTime) * 1.0e3, NANO_TO_FRAMERATE(endTime - startTime), getNanoTime());
    #ifdef PORTCE_DEBUG_ALWAYS_PACE_FRAME
        pace_frame(FRAMERATE_TO_NANO((double)60.0));
    #endif
}

void PortCE_pace_frame(float frame_rate) {
    PortCE_new_frame();
    #ifndef PORTCE_DEBUG_ALWAYS_PACE_FRAME
        pace_frame(FRAMERATE_TO_NANO((double)frame_rate));
    #endif
}
