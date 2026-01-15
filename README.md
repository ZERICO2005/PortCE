PortCE is a tool to port your Ti84-CE C/C++ application to Windows/Linux/MacOS so it can be ran as a native executable.

PortCE also allows you to add audio and mouse support, in addition to making debugging easier through the use of `gdb` and memory sanitizers.

We welcome contributions to PortCE

# PortCE requirements

* `_BitInt` support (`Clang 18` or later)
* CMake and Ninja
* `SDL2` and `SDL2_mixer`

Windows: Use Clang to compile and use `MSYS2 UCRT64`, `MSYS2 MINGW64`, or `WSL`
Linux: Use Clang to compile
MacOS: Use Clang to compile

# PortCE Setup Guide

1. Compile the library

* To start, set the `current directory` to `PortCE`
* Then run `cmake -S . -B build -G Ninja -DCMAKE_EXPORT_PACKAGE_REGISTRY=ON`
* Then PortCE can be built with `cmake --build build`

2. Building the examples

* Set the `current directory` to `PortCE/examples/hello_world`. Then run `mkdir build` and `cd build`. You should now be in `PortCE/examples/hello_world/build`.
* Then run `cmake -G Ninja ..`, and then run `Ninja`. This will compile `hello_world`.
* To run the program, do `bin/hello_world`.

# Compiling PortCE Projects

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

3. Pointers and Integers

You *MUST* ensure that you use the proper `<stdint.h>` types in your program, or you will run into protability problems.

`int` is 24 bits on the Ti84CE but is typically 32 bits on windows and linux:
- For example, `int` might be changed to: `int24_t`, `intptr_t`, or `ptrdiff_t`.
- And `unsigned int` might be changed to: `uint24_t`, `uintptr_t`, or `size_t`.

PortCE also provides `ez80_*` and `ti_*` types:
- `ez80_int` will alias `int24_t`.
- `ti_int` can be configued to alias `int24_t` or `int32_t` and is only used for libraries and routines from TICE or the CE C/C++ toolchain.

If you have a packed array of `uint24_t*`, you may have to use `packed_uint24_t*` to ensure that the pointer increments by 3 bytes instead of 4 bytes (since `sizeof(_BitInt(24))` is 4).

If you are directly accessing any pointers, you will need to wrap them in the `RAM_ADDRESS()` or `RAM_OFFSET()` macros. They should have no effect when compiled for the ti84ce.

The `void * RAM_ADDRESS(uint24_t)` macro returns a `void*` pointer to `simulated_ram[16777216]`. Example useage:
```c
// Source (Sets pixel 0,0 of the LCD to 0xFF)
*(uint8_t*)RAM_ADDRESS(0xD40000) = 0xFF;

// Compiled for Ti84-CE
*(uint8_t*)((void*)(0xD40000)) = 0xFF;

// Compiled for Windows/Linux
*(uint8_t*)((void*)&simulated_ram[0xD40000]) = 0xFF;
```

The `void * CONST_ADDRESS(uint24_t)` macro is similar to `RAM_ADDRESS()`, expect that it can be used at compile time to point to a specific address.

The `uint24_t RAM_OFFSET(void*)` macro calculates the offset from `simulated_ram[16777216]`. Example useage:
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

If you want a feature added to PortCE, feel free to contribute or create an issue https://github.com/ZERICO2005/PortCE/issues.

5. Considerations
Not all C/C++ code may work as intended. Here is a list of things to watch out for:
- `sizeof(void*)` will be 4 or 8 instead of 3.
- Integers are promoted to `int32_t` instead of `int24_t`.
- `_BitInt(24)` (used to simulate int24_t) is exempt from integer promotion rules.
- `double` is 32bits on the eZ80 instead of 64bits. This can be fixed by using `ti_double`.
- Undefined behaviour may produce different results. For example: `memcpy(buf + 4, buf, 256 - 4)` (Which fills the buffer with a repeating pattern of 4 bytes).
- Your computer will run your code *very* fast compared to the Ti84-CE, so make sure your code handles time/frame pacing correctly.
- PortCE supports compiling in C11/C++14 or later. C89, C99, C++98, and C++11 may not be supported.
- The C++ STL may behave differently from the one provided by the CE C/C++ toolchain.

Some functions have a different name to preserve behavior or avoid conflicts:
- Use `uint32_t ti_random(void)` instead of `long random(void)` (which is defined on some Linux environments)
- `clock()` typically defines `CLOCKS_PER_SEC` to be 1000. So you can use `ti_clock` and `TI_CLOCKS_PER_SEC` instead.

PortCE may run into different performance bottlenecks than the CE. For example, it takes about 1000 times as long to add two floats on the CE compared to adding two integers. But on x86-64, adding floats takes the same time as adding integers.

6. Compiling

We recommend compiling your PortCE projects with Clang (Which is the same compiler used by the CE C/C++ toolchain).

PortCE uses `_BitInt(24)` for `int24_t`, which is supported in Clang 18 onwards. Otherwise you can try using `_ExtInt(24)`. You can also try `typedef int32_t int24_t`, but this may break/crash your code.

To compile your code for PortCE, run `mkdir build`, `cd build`, and `cmake -G Ninja ..`, then run `ninja` to compile your code.

7. Keybinds

You can modify the keybinds used for PortCE by editing `PortCE/include/PortCE_Keybinds.h`

# Capabilities

## LCD/Screen
The LCD can be configured by accessing the registers/functions defined in `sys/lcd.h`,  `lcddrvce.h`, and `sys/spi.h`. Note that not all registers will have an effect, such as the LCD timings.

Supports:
* RGB/BGR and inverted colors
* 16bit Color: 1555, 565, 444
* Indexed Color: 1bit, 2bit, 4bit, and 8bit
* Partial hardware cursor support
* Row-Major and Column-Major mode
* Limited SPI support

## Extra Features

You can include `PortCE_Extra.h` to add extra functionality to the PortCE version of your program, such as mouse support.

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
