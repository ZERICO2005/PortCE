PortCE allows you to port your Ti84-CE C/C++ application to Windows/Linux so it can be ran as a native executable.

# PortCE Setup Guide

0. Files
To start, copy the contents of the PortCE folder to your project.

1. Assembly

Any assembly code will need to be commented out, or rewritten in C or C++.

You may also use `#ifdef _EZ80` to exclude code from PortCE. Conversely, `#ifndef _EZ80` can be used to exclude code from the Ti84-CE.

2. PortCE code

You will need to include PortCE.h as the **LAST** header in your code.

`void PortCE_initialize(const char* window_title)`<br>
Initialize PortCE at the beginning of your code. You may also pass in the name of your program.

`void PortCE_terminate(void)`<br>
When your program finishes, make sure to terminate PortCE.

`void PortCE_new_frame(void)`<br>
You can call this function to manually output frames.

You can add the following to your makefile so the CE C/C++ toolchain can find `PortCE.h`, along with the source code for libraries such as `graphy.c`.
```makefile
CFLAGS += -I./src_PortCE -I./src_PortCE/PortCE_include/graphy
CXXFLAGS += -I./src_PortCE -I./src_PortCE/PortCE_include/graphy

EXTRA_C_SOURCES += ./src_PortCE/PortCE_include/graphy/graphy.c
```

3. Pointers and Integers

You *MUST* convert `int`, `unsigned int`, `long`, and others to types provided by `PortCE.h`, otherwise your code may not work, or increment pointers by the wrong amount.. For example, `unsigned int` may become `ti_unsigned_int`, `uint24_t`, or `uintptr_t`. If you have a packed array of `uint24_t*`, you may have to use `packed_uint24_t*` to ensure that each element is exactly 3 bytes instead of 4 bytes.

If you are directly accessing any pointers, you will need to wrap them in the `RAM_ADDRESS()` or `RAM_OFFSET()` macros. They should have no effect when compiled for the ti84ce.

The `RAM_ADDRESS()` macro returns a `void*` pointer to `simulated_ram[16777216]`. Example useage:
```c
// Source (Sets pixel 0,0 of the LCD to 0xFF)
*(uint8_t*)RAM_ADDRESS(0xD40000) = 0xFF;

// Compiled for Ti84-CE
*(uint8_t*)((void*)(0xD40000)) = 0xFF;

// Compiled for Windows/Linux
*(uint8_t*)((void*)&simulated_ram[0xD40000]) = 0xFF;
```

The `CONST_ADDRESS()` macro is similar to `RAM_ADDRESS()`, expect that it can be used at compile time to point to a specific address.

The `RAM_OFFSET()` macro calculates the offset from `simulated_ram[16777216]`. Example useage:
```c
// Source (Resets the LCD position to the start of lcd_Ram)
lcd_Upbase = RAM_OFFSET(lcd_Ram);

// Compiled for Ti84-CE
lcd_Upbase = (uint24_t)((void*)0xD40000);

// Compiled for Windows/Linux
lcd_Upbase = ((uint24_t)((uint8_t*)((void*)&simulated_ram[0xD40000]) - (uint8_t*)simulated_ram))
```

4. Missing Libraries/Headers/Functionality

Some headers/functions may be missing, and some of the registers won't update automatically quite yet. As time goes on, more functionality will be implemented.

5. Considerations
Not all C/C++ code may work as intended. Here is a list of things to watch out for:
- `sizeof(void*)` will be 4 or 8 instead of 3.
- Integers are promoted to `int32_t` instead of `int24_t`.
- `_BitInt(24)` (used to simulate int24_t) is exempt from integer promotion rules.
- `double` is 32bits on the eZ80 instead of 64bits. This can be fixed by using `ti_double`.
- Undefined behaviour may produce different results. For example: `memcpy(buf + 4, buf, 256 - 4)` (Which fills the buffer with a repeating pattern of 4 bytes).
- Your computer will run your code *very* fast compared to the Ti84-CE, so make sure your code handles time/frame pacing correctly.

6. Compiling

PortCE uses `_BitInt(24)` for `int24_t`, so it's recommended that you use Clang C23. For reference, the CE C/C++ toolchain uses Clang C17. You may also try using `_ExtInt` in Clang.
 If your compiler does not support `_BitInt`, you can try `typedef int32_t int24_t`, but this may break/crash your code. 

To compile your code for PortCE, run `mkdir build`, `cd build`, and `cmake -G Ninja ..`, then run `ninja` to compile your code.

To compile your code for the Ti84-CE, you can use `cedev.bat` (or equivalent) normally.

# Capabilities

## LCD/Screen
The LCD is configured by accessing the registers defined in `sys/lcd.h`. Note that not all registers will have an effect, such as the LCD timings.

Supports:
* RGB and BGR
* 16bit Color: 1555, 565, 444
* Indexed Color: 1bit, 2bit, 4bit, and 8bit
* Partial hardware cursor support

Other features can be accessed through `lcddrvce.h` and `sys/spi.h`:
* Row-Major and Column-Major mode
* Inverted Colors

## Extra Features

You can include `PortCE_Extra.h` to add extra functionality to the PortCE version of your program, such as mouse support.

Additionally, you can define `PORTCE_EXTENDED_INTEGERS`, to enable the experimental `int72_t` and `int96_t` types (requires `__int128`).

# Configuration
(Unimplemented)

You can edit the PortCE.config file to modify startup behaviour, keybinds, and other settings. If a value cannot be read, the default value will be used.

## Library Settings

### clock_speed
*(1.0)*

Sets the tick speed of any clocks/timers.

### clock_date
*(Current)*

Sets the date and time when starting the program.

Options:
* Current: Uses the current date and time
* <custom>: Uses the date and time in the following format: `2015/12/31 23:59:59`

## Window Settings

### window_scale
*(2)*

Sets the initial window scale/size. If the scaled window does not fit, a smaller scaling factor will be used.

### bootup_display
*(Automatic)*

Choose which monitor the application will open to.

Options:
* Automatic: Opens on the primary display.
* <number>: Opens on a specific display. If the display is not available, display 1 will be used.
* Cursor: Opens on the display the cursor is on.
* Left/Right/Center/Top/Bottom: Opens on the display matching the direction.

### full_screen
*(false)*

Starts the application in full-screen.

## LCD Settings

### integer_scaling
*(false)*

Forces integer scaling on the LCD/Screen.

### stretch_frames
*(false)*

Stretches the frame to fit the window instead of displaying black borders.

### linear_interpolation
*(false)*

Uses linear interpolation instead of nearest neighbor interpolation. May result in a blurry image.
