/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef ENGINE_H
#define ENGINE_H

#include "../PortCE_Common.h"



/* Enum of Functions that can be activated by a KeyboardScancode */
namespace Key_Function {

	const char* const Key_Function_Text[] = {
		"NONE",
		"toggle_menu",
		"toggle_debug",
		"PORTCE",
		"key_Up",
		"key_Down",
		"key_Left",
		"key_Right",
		"DIRECTION",
		"key_2nd",
		"key_Alpha",
		"key_Enter",
		"key_Mode",
		"key_Del",
		"key_Stat",
		"key_Clear",
		"ACTION",
		"key_Graph",
		"key_Trace",
		"key_Zoom",
		"key_Window",
		"key_Yequ",
		"key_GraphVar",
		"TOP_ROW",
		"key_0",
		"key_1",
		"key_2",
		"key_3",
		"key_4",
		"key_5",
		"key_6",
		"key_7",
		"key_8",
		"key_9",
		"key_DecPnt",
		"key_Chs",
		"NUMBER",
		"key_Sto",
		"key_Ln",
		"key_Log",
		"key_Square",
		"key_Recip",
		"key_Math",
		"key_Comma",
		"key_Sin",
		"key_Apps",
		"key_LParen",
		"key_Cos",
		"key_Prgm",
		"key_RParen",
		"key_Tan",
		"key_Vars",
		"key_Add",
		"key_Sub",
		"key_Mul",
		"key_Div",
		"OTHER",
		"key_Power",
		"key_Reset",
		"POWER",
	};

	enum Key_Function_Enum {
		NONE,
		toggle_menu,
		toggle_debug,
		PORTCE,
		key_Up,
		key_Down,
		key_Left,
		key_Right,
		DIRECTION,
		key_2nd,
		key_Alpha,
		key_Enter,
		key_Mode,
		key_Del,
		key_Stat,
		key_Clear,
		ACTION,
		key_Graph,
		key_Trace,
		key_Zoom,
		key_Window,
		key_Yequ,
		key_GraphVar,
		TOP_ROW,
		key_0,
		key_1,
		key_2,
		key_3,
		key_4,
		key_5,
		key_6,
		key_7,
		key_8,
		key_9,
		key_DecPnt,
		key_Chs,
		NUMBER,
		key_Sto,
		key_Ln,
		key_Log,
		key_Square,
		key_Recip,
		key_Math,
		key_Comma,
		key_Sin,
		key_Apps,
		key_LParen,
		key_Cos,
		key_Prgm,
		key_RParen,
		key_Tan,
		key_Vars,
		key_Add,
		key_Sub,
		key_Mul,
		key_Div,
		OTHER,
		key_Power,
		key_Reset,
		POWER,
		Parameter_Function_Count,
	};
	static_assert(
		Parameter_Function_Count == ARRAY_LENGTH(Key_Function_Text),
		"Parameter_Function_Count != ARRAY_LENGTH(Key_Function_Text)"
	);

	const size_t Key_Function_Map[] = {
		NONE,
		PORTCE,
		DIRECTION,
		ACTION,
		TOP_ROW,
		NUMBER,
		OTHER,
		POWER,
	};
	const char* const Key_Function_Category_Text[] = {
		"NONE",
		"PORTCE",
		"DIRECTION",
		"ACTION",
		"TOP_ROW",
		"NUMBER",
		"OTHER",
		"POWER",
	};
	static_assert(
		ARRAY_LENGTH(Key_Function_Map) == ARRAY_LENGTH(Key_Function_Category_Text),
		"ARRAY_LENGTH(Key_Function_Map) != ARRAY_LENGTH(Key_Function_Category_Text)"
	);

	enum Key_Function_Category {
		Category_NONE,
		Category_COORDINATES,
		Category_TRANSFORMATIONS,
		Category_JULIA,
		Category_PARAMETERS,
		Category_POLAR,
		Category_FORMULA,
		// Category_SCREEN_SPLIT,
		Category_FUNCTIONS,
		Category_RENDERING
	};
};
/* String of Functions that can be activated by a KeyboardScancode */
//const char*

#endif /* ENGINE_H */
