#ifndef PORTCE_KEYBINDS_H
#define PORTCE_KEYBINDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <SDL2/SDL_scancode.h>

typedef struct PortCE_Keybind {
    uint32_t SDL;
    uint8_t CE;
} PortCE_Keybind;

typedef enum PortCE_Keycode {
    KB_Graph        , KB_Trace  , KB_Zoom   , KB_Window , KB_Yequ   , KB_2nd    , KB_Mode   , KB_Del        ,
                      KB_Sto = 9, KB_Ln     , KB_Log    , KB_Square , KB_Recip  , KB_Math   , KB_Alpha      ,
    KB_0            , KB_1      , KB_4      , KB_7      , KB_Comma  , KB_Sin    , KB_Apps   , KB_GraphVar   ,
    KB_DecPnt       , KB_2      , KB_5      , KB_8      , KB_LParen , KB_Cos    , KB_Prgm   , KB_Stat       ,
    KB_Chs          , KB_3      , KB_6      , KB_9      , KB_RParen , KB_Tan    , KB_Vars   ,
    KB_Enter = 40   , KB_Add    , KB_Sub    , KB_Mul    , KB_Div    , KB_Power  , KB_Clear  ,
    KB_Down = 48    , KB_Left   , KB_Right  , KB_Up    
} PortCE_Keycode;



/* Select which Keybinds the program will use */
#define PortCE_Keybind_Selection PortCE_Keybind_Default



static const PortCE_Keybind PortCE_Keybind_Custom[] = {

};  

static const PortCE_Keybind PortCE_Keybind_Default[] = { // Mixture of WASD-EQ-ZXC and CEmu keybinds.
    /* Keypad Numbers */
    {SDL_SCANCODE_KP_0,KB_0},{SDL_SCANCODE_KP_1,KB_1},{SDL_SCANCODE_KP_2,KB_2},{SDL_SCANCODE_KP_3,KB_3},{SDL_SCANCODE_KP_4,KB_4},
    {SDL_SCANCODE_KP_5,KB_5},{SDL_SCANCODE_KP_6,KB_6},{SDL_SCANCODE_KP_7,KB_7},{SDL_SCANCODE_KP_8,KB_8},{SDL_SCANCODE_KP_9,KB_9},
	{SDL_SCANCODE_0,KB_0},{SDL_SCANCODE_1,KB_1},{SDL_SCANCODE_2,KB_2},{SDL_SCANCODE_3,KB_3},{SDL_SCANCODE_4,KB_4},
    {SDL_SCANCODE_5,KB_5},{SDL_SCANCODE_6,KB_6},{SDL_SCANCODE_7,KB_7},{SDL_SCANCODE_8,KB_8},{SDL_SCANCODE_9,KB_9},
    /* Arrows */
    {SDL_SCANCODE_W,KB_Up},{SDL_SCANCODE_S,KB_Down},{SDL_SCANCODE_A,KB_Left},{SDL_SCANCODE_D,KB_Right},
    {SDL_SCANCODE_UP,KB_Up},{SDL_SCANCODE_DOWN,KB_Down},{SDL_SCANCODE_LEFT,KB_Left},{SDL_SCANCODE_RIGHT,KB_Right},
    /* Primary */
    {SDL_SCANCODE_E,KB_Alpha}, {SDL_SCANCODE_APOSTROPHE,KB_Alpha},
	{SDL_SCANCODE_Q,KB_2nd}, {SDL_SCANCODE_SEMICOLON,KB_2nd},
	{SDL_SCANCODE_RETURN, KB_Enter}, {SDL_SCANCODE_KP_ENTER, KB_Enter},
    /* Other */
    {SDL_SCANCODE_Z,KB_Yequ},
    {SDL_SCANCODE_LSHIFT,KB_GraphVar}, {SDL_SCANCODE_RSHIFT,KB_GraphVar},
    {SDL_SCANCODE_X,KB_Mode}, {SDL_SCANCODE_BACKSPACE, KB_Mode},
    {SDL_SCANCODE_C,KB_Power},
    {SDL_SCANCODE_P,KB_Del}, {SDL_SCANCODE_DELETE,KB_Del}
};

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_AUDIO_H */