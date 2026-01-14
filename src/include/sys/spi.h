/**
 * @file
 * @authors
 * @brief CE SPI controller define file
 * @deprecated spi.h is non-standard
 */

#ifndef SYS_SPI_H
#define SYS_SPI_H

#include <PortCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* @cond */
#define spi_ControlRegister0     ((volatile void*)RAM_ADDRESS(0xF80000))
#define spi_ControlRegister1     ((volatile void*)RAM_ADDRESS(0xF80004))
#define spi_ControlRegister2     ((volatile void*)RAM_ADDRESS(0xF80008))
#define spi_StatusBits           ((const volatile void*)RAM_ADDRESS(0xF8000C))
#define spi_InterruptControl     ((volatile void*)RAM_ADDRESS(0xF80010))
#define spi_InterruptStatus      ((const volatile void*)RAM_ADDRESS(0xF80014))
#define spi_FIFO                 ((volatile void*)RAM_ADDRESS(0xF80018))
#define spi_InsideReservedRange  ((volatile void*)RAM_ADDRESS(0xF8001C))
#define spi_Revision             ((const volatile void*)RAM_ADDRESS(0xF80060))
#define spi_Features             (*(const volatile uint32_t*)RAM_ADDRESS(0xF80064))
/* @endcond */

#ifdef _EZ80

    /**
     * In order to reliably use the LCD interface, the
     * boot_InitializeHardware routine should be called at the start of a program
     * to select the LCD interface and reset its configuration to the default.
     */
    #define boot_InitializeHardware()  ((void(*)(void))0x384)()

    /**
     * Sends a Command to the SPI controller using the 9bit FIFO protocol.
     *
     * @param[in] x 8bit command.
     */
    #define SPI_COMMAND(x) \
    do { \
        *(volatile uint8_t*)spi_FIFO = ((x) >> 6) & 0b111; \
        *(volatile uint8_t*)spi_FIFO = ((x) >> 3) & 0b111; \
        *(volatile uint8_t*)spi_FIFO = (x) & 0b111; \
        *(volatile uint8_t*)spi_ControlRegister2 = 0x1; \
        while ( ((const volatile uint8_t*)spi_StatusBits)[1] & 0xF0) {}; \
        while ( ((const volatile uint8_t*)spi_StatusBits)[0] & (1 << 2)) {}; \
        *(volatile uint8_t*)spi_ControlRegister2 = 0x0; \
    } while(0)

    /**
     * Sends a Parameter to the SPI controller using the 9bit FIFO protocol.
     *
     * @param[in] x 8bit parameter.
     */
    #define SPI_PARAMETER(x) \
    do { \
        *(volatile uint8_t*)spi_FIFO = (((x) >> 6) & 0b111) | 0b100; \
        *(volatile uint8_t*)spi_FIFO = ((x) >> 3) & 0b111; \
        *(volatile uint8_t*)spi_FIFO = (x) & 0b111; \
        *(volatile uint8_t*)spi_ControlRegister2 = 0x1; \
        while ( ((const volatile uint8_t*)spi_StatusBits)[1] & 0xF0) {}; \
        while ( ((const volatile uint8_t*)spi_StatusBits)[0] & (1 << 2)) {}; \
        *(volatile uint8_t*)spi_ControlRegister2 = 0x0; \
    } while(0)

    /** @todo Implement vsync */
    #define SPI_UNINVERT_COLORS() SPI_COMMAND(0x20)

    /** @todo Implement vsync */
    #define SPI_INVERT_COLORS() SPI_COMMAND(0x21)

    /** Sets the LCD to BGR Row-Major mode (TI-OS default) */
    #define SPI_ROW_MAJOR() \
    do { \
        SPI_COMMAND(0x36); \
        SPI_PARAMETER(0b00001000); \
        SPI_COMMAND(0x2A); \
        SPI_PARAMETER(0x00); SPI_PARAMETER(0x00); \
        SPI_PARAMETER(0x01); SPI_PARAMETER(0x3F); \
        SPI_COMMAND(0x2B); \
        SPI_PARAMETER(0x00); SPI_PARAMETER(0x00); \
        SPI_PARAMETER(0x00); SPI_PARAMETER(0xEF); \
    } while(0)

    /** Sets the LCD to BGR Column-Major mode */
    #define SPI_COLUMN_MAJOR() \
    do { \
        SPI_COMMAND(0x36); \
        SPI_PARAMETER(0b00101000); \
        SPI_COMMAND(0x2A); \
        SPI_PARAMETER(0x00); SPI_PARAMETER(0x00); \
        SPI_PARAMETER(0x00); SPI_PARAMETER(0xEF); \
        SPI_COMMAND(0x2B); \
        SPI_PARAMETER(0x00); SPI_PARAMETER(0x00); \
        SPI_PARAMETER(0x01); SPI_PARAMETER(0x3F); \
    } while(0)

#else

    #define boot_InitializeHardware()
    #define SPI_COMMAND(...)
    #define SPI_PARAMETER(...)

    void SPI_UNINVERT_COLORS(void);
    void SPI_INVERT_COLORS(void);
    void SPI_ROW_MAJOR(void);
    void SPI_COLUMN_MAJOR(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_SPI_H */
