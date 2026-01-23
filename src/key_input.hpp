#ifndef KEY_INPUT_HPP
#define KEY_INPUT_HPP

#include <PortCE_Keybinds.h>

enum class Key_Input {
    key_None,
    /* F1 - F5 */
    key_Yequ,
    key_Window,
    key_Zoom,
    key_Trace,
    key_Graph,
    /* Orthagonal UDLR */
    key_Up,
    key_Down,
    key_Left,
    key_Right,
    /* Diagonal Shift Clockwise */
    key_UpLeft,
    key_UpRight,
    key_DownLeft,
    key_DownRight,
    /* 2nd */
    key_2nd,
    key_Mode,
    key_Del,
    /* Alpha */
    key_Alpha,
    key_GraphVar,
    key_Stat,
    /* Row 4 */
    key_Math,
    key_Apps,
    key_Prgm,
    key_Vars,
    key_Clear,
    /* Row 5 */
    key_Recip,
    key_Sin,
    key_Cos,
    key_Tan,
    key_Power,
    /* Row 6 */
    key_Square,
    key_Comma,
    key_LParen,
    key_RParen,
    key_Div,
    /* Row 7 */
    key_Log,
    key_7,
    key_8,
    key_9,
    key_Mul,
    /* Row 8 */
    key_Ln,
    key_4,
    key_5,
    key_6,
    key_Sub,
    /* Row 9 */
    key_Sto,
    key_1,
    key_2,
    key_3,
    key_Add,
    /* Row 10 */
    key_On,
    key_0,
    key_DecPnt,
    key_Chs,
    key_Enter,
};

struct Key_Name {
    Key_Input code;
    const char* name;
};

const Key_Name key_name_english[] {
    /* F1 - F5 */
    {Key_Input::key_Yequ      , "y="        },
    {Key_Input::key_Window    , "window"    },
    {Key_Input::key_Zoom      , "zoom"      },
    {Key_Input::key_Trace     , "trace"     },
    {Key_Input::key_Graph     , "graph"     },
    /* Orthagonal UDLR */
    {Key_Input::key_Up        , "🡩"         },
    {Key_Input::key_Down      , "🡫"         },
    {Key_Input::key_Left      , "🡨"         },
    {Key_Input::key_Right     , "🡪"         },
    /* Diagonal Shift Clockwise */
    {Key_Input::key_UpLeft    , "🡔"         },
    {Key_Input::key_UpRight   , "🡕"         },
    {Key_Input::key_DownLeft  , "🡗"         },
    {Key_Input::key_DownRight , "🡖"         },
    /* 2nd */
    {Key_Input::key_2nd       , "2nd"       },
    {Key_Input::key_Mode      , "mode"      },
    {Key_Input::key_Del       , "del"       },
    /* Alpha */
    {Key_Input::key_Alpha     , "alpha"     },
    {Key_Input::key_GraphVar  , "xt0n"      },
    {Key_Input::key_Stat      , "stat"      },
    /* Row 4 */
    {Key_Input::key_Math      , "math"      },
    {Key_Input::key_Apps      , "apps"      },
    {Key_Input::key_Prgm      , "prgm"      },
    {Key_Input::key_Vars      , "vars"      },
    {Key_Input::key_Clear     , "clear"     },
    /* Row 5 */
    {Key_Input::key_Recip     , "x^-1"      },
    {Key_Input::key_Sin       , "sin"       },
    {Key_Input::key_Cos       , "cos"       },
    {Key_Input::key_Tan       , "tan"       },
    {Key_Input::key_Power     , "^"         },
    /* Row 6 */
    {Key_Input::key_Square    , "x^2"       },
    {Key_Input::key_Comma     , ","         },
    {Key_Input::key_LParen    , "("         },
    {Key_Input::key_RParen    , ")"         },
    {Key_Input::key_Div       , "/"         },
    /* Row 7 */
    {Key_Input::key_Log       , "log"       },
    {Key_Input::key_7         , "7"         },
    {Key_Input::key_8         , "8"         },
    {Key_Input::key_9         , "9"         },
    {Key_Input::key_Mul       , "*"         },
    /* Row 8 */
    {Key_Input::key_Ln        , "ln"        },
    {Key_Input::key_4         , "4"         },
    {Key_Input::key_5         , "5"         },
    {Key_Input::key_6         , "6"         },
    {Key_Input::key_Sub       , "-"         },
    /* Row 9 */
    {Key_Input::key_Sto       , "sto->"     },
    {Key_Input::key_1         , "1"         },
    {Key_Input::key_2         , "2"         },
    {Key_Input::key_3         , "3"         },
    {Key_Input::key_Add       , "+"         },
    /* Row 10 */
    {Key_Input::key_On        , "on"        },
    {Key_Input::key_0         , "0"         },
    {Key_Input::key_DecPnt    , "."         },
    {Key_Input::key_Chs       , "(-)"       },
    {Key_Input::key_Enter     , "enter"     },
};

const Key_Name key_name_french[] {
    /* F1 - F5 */
    {Key_Input::key_Yequ      , "f(x)"      },
    {Key_Input::key_Window    , "fenêtre"   },
    {Key_Input::key_Zoom      , "zoom"      },
    {Key_Input::key_Trace     , "trace"     },
    {Key_Input::key_Graph     , "graphe"    },
    /* Orthagonal UDLR */
    {Key_Input::key_Up        , "🡩"         },
    {Key_Input::key_Down      , "🡫"         },
    {Key_Input::key_Left      , "🡨"         },
    {Key_Input::key_Right     , "🡪"         },
    /* Diagonal Shift Clockwise */
    {Key_Input::key_UpLeft    , "🡔"         },
    {Key_Input::key_UpRight   , "🡕"         },
    {Key_Input::key_DownLeft  , "🡗"         },
    {Key_Input::key_DownRight , "🡖"         },
    /* 2nd */
    {Key_Input::key_2nd       , "2nde"      },
    {Key_Input::key_Mode      , "mode"      },
    {Key_Input::key_Del       , "suppr"     },
    /* Alpha */
    {Key_Input::key_Alpha     , "alpha"     },
    {Key_Input::key_GraphVar  , "xt0n"      },
    {Key_Input::key_Stat      , "stats"     },
    /* Row 4 */
    {Key_Input::key_Math      , "math"      },
    {Key_Input::key_Apps      , "matrice"   },
    {Key_Input::key_Prgm      , "prgm"      },
    {Key_Input::key_Vars      , "var"       },
    {Key_Input::key_Clear     , "annul"     },
    /* Row 5 */
    {Key_Input::key_Recip     , "< >"       },
    {Key_Input::key_Sin       , "trig"      },
    {Key_Input::key_Cos       , "résol"     },
    {Key_Input::key_Tan       , "-/-"       },
    {Key_Input::key_Power     , "^"         },
    /* Row 6 */
    {Key_Input::key_Square    , "x^2"       },
    {Key_Input::key_Comma     , ","         },
    {Key_Input::key_LParen    , "("         },
    {Key_Input::key_RParen    , ")"         },
    {Key_Input::key_Div       , "/"         },
    /* Row 7 */
    {Key_Input::key_Log       , "log"       },
    {Key_Input::key_7         , "7"         },
    {Key_Input::key_8         , "8"         },
    {Key_Input::key_9         , "9"         },
    {Key_Input::key_Mul       , "*"         },
    /* Row 8 */
    {Key_Input::key_Ln        , "ln"        },
    {Key_Input::key_4         , "4"         },
    {Key_Input::key_5         , "5"         },
    {Key_Input::key_6         , "6"         },
    {Key_Input::key_Sub       , "-"         },
    /* Row 9 */
    {Key_Input::key_Sto       , "sto->"     },
    {Key_Input::key_1         , "1"         },
    {Key_Input::key_2         , "2"         },
    {Key_Input::key_3         , "3"         },
    {Key_Input::key_Add       , "+"         },
    /* Row 10 */
    {Key_Input::key_On        , "on"        },
    {Key_Input::key_0         , "0"         },
    {Key_Input::key_DecPnt    , "."         },
    {Key_Input::key_Chs       , "(-)"       },
    {Key_Input::key_Enter     , "entrer"    },
};


struct Key_Box {
    Key_Input code;
    int x;
    int y;
    int width;
    int height;
    int get_x0() const {
        return x;
    }
    int get_y0() const {
        return y;
    }
    int get_x1() const {
        return x + width;
    }
    int get_y1() const {
        return y + height;
    }
    const char* get_name(bool english = true) const {
        if (!english) {
            return key_name_french[static_cast<size_t>(code)].name;
        }
        // default to English
        return key_name_english[static_cast<size_t>(code)].name;
    }
};

const Key_Box key_box[] {
    /* F1 - F5 */
    {Key_Input::key_Yequ      ,  1,  1, 4, 2},
    {Key_Input::key_Window    , 11,  1, 4, 2},
    {Key_Input::key_Zoom      , 16,  1, 4, 2},
    {Key_Input::key_Trace     , 21,  1, 4, 2},
    {Key_Input::key_Graph     , 26,  1, 4, 2},
    /* Orthagonal UDLR */
    {Key_Input::key_Up        , 19,  4, 3, 3},
    {Key_Input::key_Down      , 19, 10, 3, 3},
    {Key_Input::key_Left      , 16,  7, 3, 3},
    {Key_Input::key_Right     , 22,  7, 3, 3},
    /* Diagonal Shift Clockwise */
    {Key_Input::key_UpLeft    , 16,  4, 3, 3},
    {Key_Input::key_UpRight   , 22,  4, 3, 3},
    {Key_Input::key_DownLeft  , 16, 10, 3, 3},
    {Key_Input::key_DownRight , 22, 10, 3, 3},
    /* 2nd */
    {Key_Input::key_2nd       ,  1,  5, 4, 3},
    {Key_Input::key_Mode      ,  6,  5, 4, 3},
    {Key_Input::key_Del       , 11,  5, 4, 3},
    /* Alpha */
    {Key_Input::key_Alpha     ,  1,  9, 4, 3},
    {Key_Input::key_GraphVar  ,  6,  9, 4, 3},
    {Key_Input::key_Stat      , 11,  9, 4, 3},
    /* Row 4 */
    {Key_Input::key_Math      ,  1, 14, 4, 2},
    {Key_Input::key_Apps      ,  6, 14, 4, 2},
    {Key_Input::key_Prgm      , 11, 14, 4, 2},
    {Key_Input::key_Vars      , 16, 14, 4, 2},
    {Key_Input::key_Clear     , 21, 14, 4, 2},
    /* Row 5 */
    {Key_Input::key_Recip     ,  1, 17, 4, 2},
    {Key_Input::key_Sin       ,  6, 17, 4, 2},
    {Key_Input::key_Cos       , 11, 17, 4, 2},
    {Key_Input::key_Tan       , 16, 17, 4, 2},
    {Key_Input::key_Power     , 21, 17, 4, 2},
    /* Row 6 */
    {Key_Input::key_Square    ,  1, 20, 4, 2},
    {Key_Input::key_Comma     ,  6, 20, 4, 2},
    {Key_Input::key_LParen    , 11, 20, 4, 2},
    {Key_Input::key_RParen    , 16, 20, 4, 2},
    {Key_Input::key_Div       , 21, 20, 4, 2},
    /* Row 7 */
    {Key_Input::key_Log       ,  1, 23, 4, 2},
    {Key_Input::key_7         ,  6, 24, 4, 2},
    {Key_Input::key_8         , 11, 24, 4, 2},
    {Key_Input::key_9         , 16, 24, 4, 2},
    {Key_Input::key_Mul       , 21, 23, 4, 2},
    /* Row 8 */
    {Key_Input::key_Ln        ,  1, 26, 4, 2},
    {Key_Input::key_4         ,  6, 27, 4, 2},
    {Key_Input::key_5         , 11, 27, 4, 2},
    {Key_Input::key_6         , 16, 27, 4, 2},
    {Key_Input::key_Sub       , 21, 26, 4, 2},
    /* Row 9 */
    {Key_Input::key_Sto       ,  1, 29, 4, 2},
    {Key_Input::key_1         ,  6, 30, 4, 2},
    {Key_Input::key_2         , 11, 30, 4, 2},
    {Key_Input::key_3         , 16, 30, 4, 2},
    {Key_Input::key_Add       , 21, 29, 4, 2},
    /* Row 10 */
    {Key_Input::key_On        ,  1, 32, 4, 2},
    {Key_Input::key_0         ,  6, 33, 4, 2},
    {Key_Input::key_DecPnt    , 11, 33, 4, 2},
    {Key_Input::key_Chs       , 16, 33, 4, 2},
    {Key_Input::key_Enter     , 21, 32, 4, 2},
};

inline PortCE_Keycode to_PortCE_Keycode(Key_Input code) {
    switch (code) {
        case Key_Input::key_Down    : return KB_Down     ;
        case Key_Input::key_Left    : return KB_Left     ;
        case Key_Input::key_Right   : return KB_Right    ;
        case Key_Input::key_Up      : return KB_Up       ;
        case Key_Input::key_Enter   : return KB_Enter    ;
        case Key_Input::key_2nd     : return KB_2nd      ;
        case Key_Input::key_Clear   : return KB_Clear    ;
        case Key_Input::key_Alpha   : return KB_Alpha    ;
        case Key_Input::key_Add     : return KB_Add      ;
        case Key_Input::key_Sub     : return KB_Sub      ;
        case Key_Input::key_Mul     : return KB_Mul      ;
        case Key_Input::key_Div     : return KB_Div      ;
        case Key_Input::key_Graph   : return KB_Graph    ;
        case Key_Input::key_Trace   : return KB_Trace    ;
        case Key_Input::key_Zoom    : return KB_Zoom     ;
        case Key_Input::key_Window  : return KB_Window   ;
        case Key_Input::key_Yequ    : return KB_Yequ     ;
        case Key_Input::key_Mode    : return KB_Mode     ;
        case Key_Input::key_Del     : return KB_Del      ;
        case Key_Input::key_Sto     : return KB_Sto      ;
        case Key_Input::key_Ln      : return KB_Ln       ;
        case Key_Input::key_Log     : return KB_Log      ;
        case Key_Input::key_Square  : return KB_Square   ;
        case Key_Input::key_Recip   : return KB_Recip    ;
        case Key_Input::key_Math    : return KB_Math     ;
        case Key_Input::key_0       : return KB_0        ;
        case Key_Input::key_1       : return KB_1        ;
        case Key_Input::key_4       : return KB_4        ;
        case Key_Input::key_7       : return KB_7        ;
        case Key_Input::key_2       : return KB_2        ;
        case Key_Input::key_5       : return KB_5        ;
        case Key_Input::key_8       : return KB_8        ;
        case Key_Input::key_3       : return KB_3        ;
        case Key_Input::key_6       : return KB_6        ;
        case Key_Input::key_9       : return KB_9        ;
        case Key_Input::key_Comma   : return KB_Comma    ;
        case Key_Input::key_Sin     : return KB_Sin      ;
        case Key_Input::key_Apps    : return KB_Apps     ;
        case Key_Input::key_GraphVar: return KB_GraphVar ;
        case Key_Input::key_DecPnt  : return KB_DecPnt   ;
        case Key_Input::key_LParen  : return KB_LParen   ;
        case Key_Input::key_Cos     : return KB_Cos      ;
        case Key_Input::key_Prgm    : return KB_Prgm     ;
        case Key_Input::key_Stat    : return KB_Stat     ;
        case Key_Input::key_Chs     : return KB_Chs      ;
        case Key_Input::key_RParen  : return KB_RParen   ;
        case Key_Input::key_Tan     : return KB_Tan      ;
        case Key_Input::key_Vars    : return KB_Vars     ;
        case Key_Input::key_Power   : return KB_Power    ;
        default: return KB_None;
    }
}

#ifdef __cplusplus
extern "C" {
#endif
void Key_Input_key_down(Key_Input code);
void Key_Input_key_up(Key_Input code);
#ifdef __cplusplus
}
#endif

#endif /* KEY_INPUT_HPP */
