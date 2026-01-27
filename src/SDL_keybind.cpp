#include "SDL_keybind.hpp"

static const SDL_Keybind Default_Keybind {
    /* Keypad Numbers */
    {Key_Input::key_0         , SDL_SCANCODE_KP_0},
    {Key_Input::key_1         , SDL_SCANCODE_KP_1},
    {Key_Input::key_2         , SDL_SCANCODE_KP_2},
    {Key_Input::key_3         , SDL_SCANCODE_KP_3},
    {Key_Input::key_4         , SDL_SCANCODE_KP_4},
    {Key_Input::key_5         , SDL_SCANCODE_KP_5},
    {Key_Input::key_6         , SDL_SCANCODE_KP_6},
    {Key_Input::key_7         , SDL_SCANCODE_KP_7},
    {Key_Input::key_8         , SDL_SCANCODE_KP_8},
    {Key_Input::key_9         , SDL_SCANCODE_KP_9},
    {Key_Input::key_0         , SDL_SCANCODE_0},
    {Key_Input::key_1         , SDL_SCANCODE_1},
    {Key_Input::key_2         , SDL_SCANCODE_2},
    {Key_Input::key_3         , SDL_SCANCODE_3},
    {Key_Input::key_4         , SDL_SCANCODE_4},
    {Key_Input::key_5         , SDL_SCANCODE_5},
    {Key_Input::key_6         , SDL_SCANCODE_6},
    {Key_Input::key_7         , SDL_SCANCODE_7},
    {Key_Input::key_8         , SDL_SCANCODE_8},
    {Key_Input::key_9         , SDL_SCANCODE_9},
    /* Keypad Symbols */
    {Key_Input::key_Add       , SDL_SCANCODE_KP_PLUS},
    {Key_Input::key_Sub       , SDL_SCANCODE_KP_MINUS},
    {Key_Input::key_Mul       , SDL_SCANCODE_KP_MULTIPLY},
    {Key_Input::key_Div       , SDL_SCANCODE_KP_DIVIDE},
    {Key_Input::key_DecPnt    , SDL_SCANCODE_KP_PERIOD},
    /* Arrows */
    {Key_Input::key_Up        , SDL_SCANCODE_W},
    {Key_Input::key_Down      , SDL_SCANCODE_S},
    {Key_Input::key_Left      , SDL_SCANCODE_A},
    {Key_Input::key_Right     , SDL_SCANCODE_D},
    {Key_Input::key_Up        , SDL_SCANCODE_UP},
    {Key_Input::key_Down      , SDL_SCANCODE_DOWN},
    {Key_Input::key_Left      , SDL_SCANCODE_LEFT},
    {Key_Input::key_Right     , SDL_SCANCODE_RIGHT},
    /* Primary */
    {Key_Input::key_Alpha     , SDL_SCANCODE_E},
    {Key_Input::key_Alpha     , SDL_SCANCODE_APOSTROPHE},
    {Key_Input::key_2nd       , SDL_SCANCODE_Q},
    {Key_Input::key_2nd       , SDL_SCANCODE_SEMICOLON},
    {Key_Input::key_Enter     , SDL_SCANCODE_RETURN},
    {Key_Input::key_Enter     , SDL_SCANCODE_KP_ENTER},
    /* Other */
    {Key_Input::key_Yequ      , SDL_SCANCODE_Z},
    {Key_Input::key_GraphVar  , SDL_SCANCODE_LSHIFT},
    {Key_Input::key_GraphVar  , SDL_SCANCODE_RSHIFT},
    {Key_Input::key_Mode      , SDL_SCANCODE_X},
    {Key_Input::key_Mode      , SDL_SCANCODE_BACKSPACE},
    {Key_Input::key_Clear     , SDL_SCANCODE_C},
    {Key_Input::key_Clear     , SDL_SCANCODE_ESCAPE},
    {Key_Input::key_Del       , SDL_SCANCODE_P},
    {Key_Input::key_Del       , SDL_SCANCODE_DELETE},
    {Key_Input::key_Power     , SDL_SCANCODE_GRAVE},
    /* Functions */
    {Key_Input::key_Graph     , SDL_SCANCODE_F1},
    {Key_Input::key_Trace     , SDL_SCANCODE_F2},
    {Key_Input::key_Zoom      , SDL_SCANCODE_F3},
    {Key_Input::key_Window    , SDL_SCANCODE_F4},
    {Key_Input::key_Yequ      , SDL_SCANCODE_F5},
};

const SDL_Keybind& get_Default_Keybind() {
    return Default_Keybind;
}

const SDL_Keybind& get_Current_Keybind() {
    return get_Default_Keybind();
}
