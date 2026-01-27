/**
 * @file
 *
 * @author "zerico2005"
 */

#include "PortCE_Common.h"
#include "nano_time.hpp"
#include "PortCE_Render.h"
#include "PortCE_internal.h"
#include <PortCE.h>

#include <PortCE_Extra.h>
#include <sys/lcd.h>

#include <lcddrvce.h>
#include "PortCE_SPI.h"
#include <cstdio>

#include "frame_manipulation.hpp"

#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

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

static uint8_t videoCopy[153600];
static uint16_t paletteRAM[256];

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
static void copyFrame(uint32_t* data) {
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
    const uint8_t *vram = static_cast<const uint8_t*>(RAM_ADDRESS(0xD00000 | (lcd_UpBase & (0xFFFF << 3))));
    memcpy(videoCopy, vram, copyAmount);
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
        SDL_Event event;
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
    SDL_Event event;
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
