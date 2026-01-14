/**
 * @file
 *
 * @author "zerico2005"
 */

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h>

#include "PortCE_Common.h"
#include "PortCE_Render.h"

#include "PortCE_SPI.h"
#include <sys/lcd.h>
#include <lcddrvce.h>

#include "PortCE.h"

SPI_State PortCE_SPI_State;

/* SPI_Commands */

/** Command `0x00` */
static const void* SPI_no_operation(SPI_State* const state, const void* const args) {
    // Flush SPI command queue
    (void)state;
    return args;
}

/** Command `0x01` */
static const void* SPI_software_reset(SPI_State* const state, const void* const args) {
    (void)state;
    return args;
}

/** Command `0x10` */
static const void* SPI_sleep_IN(SPI_State* const state, const void* const args) {
    (void)state;
    return args;
}

/** Command `0x11` */
static const void* SPI_sleep_OUT(SPI_State* const state, const void* const args) {
    (void)state;
    return args;
}

/** Command `0x12` */
static const void* SPI_partial_display_mode_ON(SPI_State* const state, const void* const args) {
    state->vertical_scroll_start_addr = 0;
    return args;
}

/** Command `0x13` */
static const void* SPI_normal_display_mode_ON(SPI_State* const state, const void* const args) {
    state->vertical_scroll_start_addr = 0;
    return args;
}

/** Command `0x20` */
static const void* SPI_display_inversion_OFF(SPI_State* const state, const void* const args) {
    PortCE_invert_colors = state->LCM_control.invert_display_inversion ? true : false;
    return args;
}

/** Command `0x21` */
static const void* SPI_display_inversion_ON(SPI_State* const state, const void* const args) {
    PortCE_invert_colors = state->LCM_control.invert_display_inversion ? false : true;
    return args;
}

/** Command `0x26` */
static const void* SPI_gamma_set(SPI_State* const state, const void* const args) {
    const uint8_t curve = *((const uint8_t*)args);
    switch (curve) {
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
            state->gamma_curve = curve;
        default:
            state->gamma_curve = 0x1;
    }
    return (const void*)((const uint8_t*)args + sizeof(uint8_t));
}

/** Command `0x28` */
static const void* SPI_display_OFF(SPI_State* const state, const void* const args) {
    (void)state;
    return args;
}

/** Command `0x29` */
static const void* SPI_display_ON(SPI_State* const state, const void* const args) {
    (void)state;
    return args;
}

/** Command `0x2A` */
static const void* SPI_column_address_set(SPI_State* const state, const void* const args) {
    state->X0_addr = ((const uint16_t*)args)[0];
    state->X1_addr = ((const uint16_t*)args)[1];
    return (const void*)((const uint8_t*)args + sizeof(uint16_t) * 3);
}

/** Command `0x2B` */
static const void* SPI_row_address_set(SPI_State* const state, const void* const args) {
    state->Y0_addr = ((const uint16_t*)args)[0];
    state->Y1_addr = ((const uint16_t*)args)[1];
    return (const void*)((const uint8_t*)args + sizeof(uint16_t) * 3);
}

/** Command `0x2C` */
static const void* SPI_write_memory(SPI_State* const state, const void* const args) {
    (void)state;
    return args;
}

/** Command `0x30` */
static const void* SPI_partial_area(SPI_State* const state, const void* const args) {
    state->partial_area.partial_start_line = ((const uint16_t*)args)[0];
    state->partial_area.partial_end_line   = ((const uint16_t*)args)[1];
    return (const void*)((const uint8_t*)args + sizeof(uint16_t) * 2);
}

/** Command `0x33` */
static const void* SPI_vertical_scrolling_definition(SPI_State* const state, const void* const args) {
    state->vertical_scrolling_definition.top_fixed_area          = ((const uint16_t*)args)[0];
    state->vertical_scrolling_definition.virtical_scrolling_area = ((const uint16_t*)args)[1];
    state->vertical_scrolling_definition.bottom_fixed_area       = ((const uint16_t*)args)[2];
    return (const void*)((const uint8_t*)args + sizeof(uint16_t) * 3);
}

/** Command `0x36` */
static const void* SPI_memory_data_access_control(SPI_State* const state, const void* const args) {
    const size_t copy_size = sizeof(state->memory_data_access);
    memcpy(&state->memory_data_access, args, copy_size);
    return (const void*)((const uint8_t*)args + copy_size);
}

/** Command `0x37` */
static const void* SPI_vertical_scroll_start_address(SPI_State* const state, const void* const args) {
    state->vertical_scroll_start_addr = *((const uint16_t*)args);
    return (const void*)((const uint8_t*)args + sizeof(uint16_t));
}

/** Command `0x38` */
static const void* SPI_idle_mode_OFF(SPI_State* const state, const void* const args) {
    PortCE_color_idle_mode = false;
    (void)state;
    return args;
}

/** Command `0x39` */
static const void* SPI_idle_mode_ON(SPI_State* const state, const void* const args) {
    PortCE_color_idle_mode = true;
    (void)state;
    return args;
}

/** Command `0x3A` */
static const void* SPI_interface_pixel_format(SPI_State* const state, const void* const args) {
    const size_t copy_size = sizeof(state->interface_pixel_format);
    memcpy(&state->interface_pixel_format, args, copy_size);
    return (const void*)((const uint8_t*)args + copy_size);
}

/** Command `0x3C` */
static const void* SPI_write_memory_continue(SPI_State* const state, const void* const args) {
    (void)state;
    return args;
}

/** Command `0xB0` */
static const void* SPI_RAM_control(SPI_State* const state, const void* const args) {
    const size_t copy_size = sizeof(state->RAM_control);
    memcpy(&state->RAM_control, args, copy_size);
    return (const void*)((const uint8_t*)args + copy_size);
}

/** Command `0xB1` */
static const void* SPI_RGB_interface_control(SPI_State* const state, const void* const args) {
    const size_t copy_size = sizeof(state->RGB_interface);
    memcpy(&state->RGB_interface, args, copy_size);
    return (const void*)((const uint8_t*)args + copy_size);
}

/** Command `0xB2` */
static const void* SPI_porch_setting(SPI_State* const state, const void* const args) {
    const size_t copy_size = sizeof(state->porch_setting);
    memcpy(&state->porch_setting, args, copy_size);
    return (const void*)((const uint8_t*)args + copy_size);
}

/** Command `0xB3` */
static const void* SPI_frame_rate_control_1(SPI_State* const state, const void* const args) {
    const size_t copy_size = sizeof(state->frame_rate_control_1);
    memcpy(&state->frame_rate_control_1, args, copy_size);
    return (const void*)((const uint8_t*)args + copy_size);
}

/** Command `0xB5` */
static const void* SPI_partial_control(SPI_State* const state, const void* const args) {
    const size_t copy_size = sizeof(state->partial_control);
    memcpy(&state->partial_control, args, copy_size);
    return (const void*)((const uint8_t*)args + copy_size);
}

// /** Command `0xB7` */
// static const void* SPI_gate_control(SPI_State* const state, const void* const args) {
//     const size_t copy_size = sizeof(state->gate_control);
//     memcpy(&state->gate_control, args, copy_size);
//     return (const void*)((const uint8_t*)args + copy_size);
// }

/** Command `0xBA` */
static const void* SPI_digital_gamma_enable(SPI_State* const state, const void* const args) {
    state->digital_gamma_enable = *((const uint8_t*)args);
    return (const void*)((const uint8_t*)args + sizeof(uint8_t));
}

// /** Command `0xBB` */
// static const void* SPI_VCOM_setting(SPI_State* const state, const void* const args) {
//     state->VCOM_setting = *((const uint8_t*)args);
//     return (const void*)((const uint8_t*)args + sizeof(uint8_t));
// }

/** Command `0xC0` */
static const void* SPI_LCM_control(SPI_State* const state, const void* const args) {
    const size_t copy_size = sizeof(state->LCM_control);
    memcpy(&state->LCM_control, args, sizeof(state->LCM_control));
    return (const void*)((const uint8_t*)args + copy_size);
}

/* Query SPI */

void PortCE_reset_SPI_state(const bool TiOS_defaults) {
    PortCE_SPI_State.gamma_curve = TiOS_defaults ? 0x2 : 0x1;

    lcd_SetColumnAddress(0, (TiOS_defaults ? LCD_RESX : LCD_RESY) - 1);
    lcd_SetRowAddress   (0, (TiOS_defaults ? LCD_RESY : LCD_RESX) - 1);

    PortCE_SPI_State.memory_data_access.display_data_latch_order = 0;
    PortCE_SPI_State.memory_data_access.BGR_order                = TiOS_defaults ? 1 : 0;
    PortCE_SPI_State.memory_data_access.line_addr_order          = 0;
    PortCE_SPI_State.memory_data_access.page_column_order        = 0;
    PortCE_SPI_State.memory_data_access.column_addr_order        = 0;
    PortCE_SPI_State.memory_data_access.page_addr_order          = 0;

    PortCE_SPI_State.vertical_scroll_start_addr = 0;

    PortCE_SPI_State.interface_pixel_format.SPI_format = 0x6;
    PortCE_SPI_State.interface_pixel_format.RGB_format = 0x6;

    PortCE_SPI_State.RAM_control.display_operating_mode         = TiOS_defaults ? 0x1 : 0x0;
    PortCE_SPI_State.RAM_control.RAM_access_selection           = TiOS_defaults ? 1 : 0;
    PortCE_SPI_State.RAM_control.data_transfer_method           = 0x0;
    PortCE_SPI_State.RAM_control.RGB_interface_bus_width        = 0;
    PortCE_SPI_State.RAM_control.endianness                     = 0;
    PortCE_SPI_State.RAM_control.expanded_pixel_format          = 0x3;
    PortCE_SPI_State.RAM_control.SPI_interface_wrap_at_end_mode = 1;
    PortCE_SPI_State.RAM_control.RGB_interface_wrap_at_end_mode = 1;

    PortCE_SPI_State.RGB_interface.enable_polarity  = 1;
    PortCE_SPI_State.RGB_interface.dotclk_polarity  = 0;
    PortCE_SPI_State.RGB_interface.hsync_polarity   = 0;
    PortCE_SPI_State.RGB_interface.vsync_polarity   = 0;
    PortCE_SPI_State.RGB_interface.data_enable_mode = TiOS_defaults ? 0x2 : 0x0;
    PortCE_SPI_State.RGB_interface.direct_RGB_mode  = 0;
    PortCE_SPI_State.RGB_interface.vsync_back_porch = TiOS_defaults ? 0x05 : 0x02;
    PortCE_SPI_State.RGB_interface.hsync_back_porch = TiOS_defaults ? 0x14 : 0x14;

    PortCE_SPI_State.porch_setting.normal_mode_back_porch_duration   = 0xC;
    PortCE_SPI_State.porch_setting.normal_mode_front_porch_duration  = 0xC;
    PortCE_SPI_State.porch_setting.separate_porch_control_enable     = 0;
    PortCE_SPI_State.porch_setting.idle_mode_back_porch_duration     = 0x3;
    PortCE_SPI_State.porch_setting.idle_mode_front_porch_duration    = 0x3;
    PortCE_SPI_State.porch_setting.partial_mode_back_porch_duration  = 0x3;
    PortCE_SPI_State.porch_setting.partial_mode_front_porch_duration = 0x3;

    PortCE_SPI_State.frame_rate_control_1.frame_rate_divisor                     = 0x0;
    PortCE_SPI_State.frame_rate_control_1.separate_frame_rate_control_enable     = 0;
    PortCE_SPI_State.frame_rate_control_1.idle_mode_frame_rate_control           = 0xF;
    PortCE_SPI_State.frame_rate_control_1.idle_mode_polarity_inversion_method    = 0x0;
    PortCE_SPI_State.frame_rate_control_1.partial_mode_frame_rate_control        = 0xF;
    PortCE_SPI_State.frame_rate_control_1.partial_mode_polarity_inversion_method = 0x0;

    PortCE_SPI_State.partial_control.interval_scan_cycle_length  = 0x0;
    PortCE_SPI_State.partial_control.interval_scan_enable        = 0;
    PortCE_SPI_State.partial_control.non_display_level           = 0;

    PortCE_SPI_State.gate_control.VGL_setting = 0x5; // -10.43V
    PortCE_SPI_State.gate_control.VGH_setting = 0x3; // +13.26V

    PortCE_SPI_State.digital_gamma_enable = 0;
    PortCE_SPI_State.VCOM_setting         = TiOS_defaults ? 0x20 : 0x17; // +0.9V or +0.675V

    PortCE_SPI_State.LCM_control.invert_gate_control             = 0;
    PortCE_SPI_State.LCM_control.invert_page_column_order        = TiOS_defaults ? 1 : 0;
    PortCE_SPI_State.LCM_control.invert_display_data_latch_order = TiOS_defaults ? 0 : 1;
    PortCE_SPI_State.LCM_control.invert_column_address_order     = TiOS_defaults ? 0 : 1;
    PortCE_SPI_State.LCM_control.invert_display_inversion        = 0;
    PortCE_SPI_State.LCM_control.invert_BGR_order                = 1;
    PortCE_SPI_State.LCM_control.invert_page_addr_order          = 0;
}

/* Query SPI */

bool PortCE_query_column_major(void) {
    return !(
        PortCE_SPI_State.memory_data_access.page_column_order !=
        PortCE_SPI_State.LCM_control.invert_page_column_order
    );
}

/* <lcddrvce.h> */

void lcd_Init(void) {
    PortCE_update_registers();
    PortCE_reset_SPI_state(true);
    return;
}

void lcd_Wait(void) {
    PortCE_update_registers();
    return;
}

void lcd_Cleanup(void) {
    PortCE_update_registers();
    return;
}

#if 0
void lcd_SetInvertedMode(bool on) {
    PortCE_invert_colors = on;
}

void lcd_SetIdleMode(bool on) {
    PortCE_color_idle_mode = on;
}

void lcd_SetColumnAddress(uint16_t XS, uint16_t XE) {
    if (XS > XE || XS > LCD_RESX || XE >= LCD_RESX) {
        printf("lcd_SetColumnAddress invalid parameters %" PRIu16 " %" PRIu16 "\n", XS, XE);
        return;
    }
    PortCE_SPI_State.X0_addr = XS;
    PortCE_SPI_State.X1_addr = XE;
}
void lcd_SetRowAddress(uint16_t YS, uint16_t YE) {
    if (YS > YE || YS > LCD_RESX || YE >= LCD_RESX) {
        printf("lcd_SetColumnAddress invalid parameters %" PRIu16 " %" PRIu16 "\n", YS, YE);
        return;
    }
    PortCE_SPI_State.Y0_addr = YS;
    PortCE_SPI_State.Y1_addr = YE;
}

void lcd_SetColumnMajor(bool on) {
    PortCE_column_major = on;
    lcd_SetColumnAddress(0, on ? 239 : 319);
    lcd_SetRowAddress(0, on ? 319 : 239);
}

/** @todo validate inputs */
void lcd_SetPartialArea(uint16_t PSL, uint16_t PEL) {
    PortCE_SPI_State.partial_area.partial_start_line = PSL;
    PortCE_SPI_State.partial_area.partial_end_line = PEL;
}

/** @todo validate inputs */
void lcd_SetScrollArea(uint16_t TFA, uint16_t VSA, uint16_t BFA) {
    PortCE_SPI_State.vertical_scrolling_definition.top_fixed_area = TFA;
    PortCE_SPI_State.vertical_scrolling_definition.virtical_scrolling_area = VSA;
    PortCE_SPI_State.vertical_scrolling_definition.bottom_fixed_area = BFA;
}

void lcd_SetScrollAddress(uint16_t VSP_addr) {
    PortCE_SPI_State.vertical_scroll_start_addr = VSP_addr;
}
#endif

const void* lcd_SendSizedCommandRaw(uint16_t sized_cmd, const void* params) {
    const uint8_t command = (uint8_t)(sized_cmd & 0xFF);
    SPI_State* const state = &PortCE_SPI_State;
    switch (command) {
        case LCD_CMD_NOP       :  /**< No operation */
            params = SPI_no_operation(state, params);
            break;
        case LCD_CMD_SWRESET   :  /**< Software reset */
            params = SPI_software_reset(state, params);
            break;
        // case LCD_CMD_RDDID     :  /**< Read display ID */
        //     break;
        // case LCD_CMD_RDDST     :  /**< Read display status */
        //     break;
        // case LCD_CMD_RDDPM     :  /**< Read display power mode */
        //     break;
        // case LCD_CMD_RDDMADCTL :  /**< Read display MADCTL */
        //     break;
        // case LCD_CMD_RDDCOLMOD :  /**< Read display pixel format */
        //     break;
        // case LCD_CMD_RDDIM     :  /**< Read display image mode */
        //     break;
        // case LCD_CMD_RDDSM     :  /**< Read display signal mode */
        //     break;
        // case LCD_CMD_RDDSDR    :  /**< Read display self-diagnostic result */
        //     break;
        case LCD_CMD_SLPIN     :  /**< Sleep in */
            params = SPI_sleep_IN(state, params);
            break;
        case LCD_CMD_SLPOUT    :  /**< Sleep out */
            params = SPI_sleep_OUT(state, params);
            break;
        case LCD_CMD_PTLON     :  /**< Partial display mode on */
            params = SPI_partial_display_mode_ON(state, params);
            break;
        case LCD_CMD_NORON     :  /**< Normal display mode on */
            params = SPI_normal_display_mode_ON(state, params);
            break;
        case LCD_CMD_INVOFF    :  /**< Display inversion off */
            params = SPI_display_inversion_OFF(state, params);
            break;
        case LCD_CMD_INVON     :  /**< Display inversion on */
            params = SPI_display_inversion_ON(state, params);
            break;
        case LCD_CMD_GAMSET    :  /**< Gamma set */
            params = SPI_gamma_set(state, params);
            break;
        case LCD_CMD_DISPOFF   :  /**< Display off */
            params = SPI_display_OFF(state, params);
            break;
        case LCD_CMD_DISPON    :  /**< Display on */
            params = SPI_display_ON(state, params);
            break;
        case LCD_CMD_CASET     :  /**< Column address set */
            params = SPI_column_address_set(state, params);
            break;
        case LCD_CMD_RASET     :  /**< Row address set */
            params = SPI_row_address_set(state, params);
            break;
        case LCD_CMD_RAMWR     :  /**< Memory write */
            params = SPI_write_memory(state, params);
            break;
        // case LCD_CMD_RAMRD     :  /**< Memory read */
        //     break;
        case LCD_CMD_PTLAR     :  /**< Partial area */
            params = SPI_partial_area(state, params);
            break;
        case LCD_CMD_VSCRDEF   :  /**< Vertical scrolling definition */
            params = SPI_vertical_scrolling_definition(state, params);
            break;
        // case LCD_CMD_TEOFF     :  /**< Tearing effect line off */
        //     break;
        // case LCD_CMD_TEON      :  /**< Tearing effect line on */
        //     break;
        case LCD_CMD_MADCTL    :  /**< Memory data access control */
            params = SPI_memory_data_access_control(state, params);
            break;
        case LCD_CMD_VSCSAD    :  /**< Vertical scroll start address of RAM */
            params = SPI_vertical_scroll_start_address(state, params);
            break;
        case LCD_CMD_IDMOFF    :  /**< Idle mode off */
            params = SPI_idle_mode_OFF(state, params);
            break;
        case LCD_CMD_IDMON     :  /**< Idle mode on */
            params = SPI_idle_mode_ON(state, params);
            break;
        case LCD_CMD_COLMOD    :  /**< Interface pixel format */
            params = SPI_interface_pixel_format(state, params);
            break;
        case LCD_CMD_RAMWRC    :  /**< Write memory continue */
            params = SPI_write_memory_continue(state, params);
            break;
        // case LCD_CMD_RAMRDC    :  /**< Read memory continue */
        //     break;
        // case LCD_CMD_STE       :  /**< Set tear scanline */
        //     break;
        // case LCD_CMD_GSCAN     :  /**< Get scanline */
        //     break;
        // case LCD_CMD_WRDISBV   :  /**< Write display brightness */
        //     break;
        // case LCD_CMD_RDDISBV   :  /**< Read display brightness */
        //     break;
        // case LCD_CMD_WRCTRLD   :  /**< Write CTRL display */
        //     break;
        // case LCD_CMD_RDCTRLD   :  /**< Read CTRL value display */
        //     break;
        // case LCD_CMD_WRCACE    :  /**< Write content adaptive brightness control and color enhancement */
        //     break;
        // case LCD_CMD_RDCABC    :  /**< Read content adaptive brightness control */
        //     break;
        // case LCD_CMD_WRCABCMB  :  /**< Write CABC minimum brightness */
        //     break;
        // case LCD_CMD_RDCABCMB  :  /**< Read CABC minimum brightness */
        //     break;
        // case LCD_CMD_RDABCSDR  :  /**< Read automatic brightness control self-diagnostic result */
        //     break;
        case LCD_CMD_RAMCTRL   :  /**< RAM control */
            params = SPI_RAM_control(state, params);
            break;
        case LCD_CMD_RGBCTRL   :  /**< RGB interface control */
            params = SPI_RGB_interface_control(state, params);
            break;
        case LCD_CMD_PORCTRL   :  /**< Porch setting */
            params = SPI_porch_setting(state, params);
            break;
        case LCD_CMD_FRCTRL1   :  /**< Frame rate control 1 (in partial mode/idle colors) */
            params = SPI_frame_rate_control_1(state, params);
            break;
        case LCD_CMD_PARCTRL   :  /**< Partial control */
            params = SPI_partial_control(state, params);
            break;
        // case LCD_CMD_GCTRL     :  /**< Gate control */
        //     params = SPI_gate_control(state, params);
        //     break;
        case LCD_CMD_GTADJ     :  /**< Gate on timing adjustment */
            // break;
        case LCD_CMD_DGMEN     :  /**< Digital gamma enable */
            params = SPI_digital_gamma_enable(state, params);
            break;
        // case LCD_CMD_VCOMS     :  /**< VCOM setting */
        //     params = SPI_VCOM_setting(state, params);
        //     break;
        case LCD_CMD_POWSAVE   :  /**< Power saving mode */
            break;
        case LCD_CMD_DLPOFFSAVE:  /**< Display off power save */
            break;
        case LCD_CMD_LCMCTRL   :  /**< LCM control */
            params = SPI_LCM_control(state, params);
            break;
        case LCD_CMD_IDSET     :  /**< ID code setting */
            break;
        // case LCD_CMD_VDVVRHEN  :  /**< VDV and VRH command enable */
        //     break;
        // case LCD_CMD_VRHS      :  /**< VRH set */
        //     break;
        // case LCD_CMD_VDV       :  /**< VDV set */
        //     break;
        case LCD_CMD_VCMOFSET  :  /**< VCOM offset set */
            break;
        case LCD_CMD_FRCTRL2   :  /**< Frame rate control in normal mode */
            break;
        // case LCD_CMD_CABCCTRL  :  /**< CABC control */
        //     break;
        // case LCD_CMD_REGSEL1   :  /**< Register value selection 1 */
        //     break;
        // case LCD_CMD_REGSEL2   :  /**< Register value selection 2 */
        //     break;
        // case LCD_CMD_PWMFRSEL  :  /**< PWM frequency selection */
        //     break;
        // case LCD_CMD_PWCTRL1   :  /**< Power control 1 */
        //     break;
        // case LCD_CMD_VAPVANEN  :  /**< Enable VAP/VAN signal output */
        //     break;
        // case LCD_CMD_RDID1     :  /**< Read ID1 */
        //     break;
        // case LCD_CMD_RDID2     :  /**< Read ID2 */
        //     break;
        // case LCD_CMD_RDID3     :  /**< Read ID3 */
        //     break;
        // case LCD_CMD_CMD2EN    :  /**< Command 2 enable */
        //     break;
        case LCD_CMD_PVGAMCTRL :  /**< Positive voltage gamma control */
            break;
        case LCD_CMD_NVGAMCTRL :  /**< Negative voltage gamma control */
            break;
        case LCD_CMD_DGMLUTR   :  /**< Digital gamma lookup table for red */
            break;
        case LCD_CMD_DGMLUTB   :  /**< Digital gamma lookup table for blue */
            break;
        case LCD_CMD_GATECTRL  :  /**< Gate control */
            break;
        // case LCD_CMD_SPI2EN    :  /**< SPI2 enable */
        //     break;
        // case LCD_CMD_PWCTRL2   :  /**< Power control 2 */
        //     break;
        case LCD_CMD_EQCTRL    :  /**< Equalize time control */
            break;
        // case LCD_CMD_PROMCTRL  :  /**< Program mode control */
        //     break;
        // case LCD_CMD_PROMEN    :  /**< Program mode enable */
        //     break;
        // case LCD_CMD_NVMSET    :  /**< NVM setting */
        //     break;
        // case LCD_CMD_PROMACT   :  /**< Program action */
        //     break;
        default:
            break;
    }
    return params;
}
const void *lcd_SendParamsRaw(
    __attribute__((unused)) size_t size,
    __attribute__((unused)) const void *params
) {
    return nullptr;
}
void lcd_SendCommand(uint8_t cmd) {
    lcd_SendSizedCommandRaw(cmd | (sizeof(uint8_t) << 8), (const void*)&cmd);
}
void lcd_SendCommand1(uint8_t cmd, uint8_t param) {
    lcd_SendSizedCommandRaw(cmd | (sizeof(uint8_t) << 8), (const void*)&param);
}
void lcd_SendCommand2(uint8_t cmd, uint8_t param1, uint8_t param2) {
    uint8_t params[2] = {param1, param2};
    lcd_SendSizedCommandRaw(cmd | ((sizeof(uint8_t) * 2) << 8), (const void*)&params);
}
void lcd_SendSizedCommandBytes(unsigned int sized_cmd, ...) {
    uint8_t params[256];
    va_list args;
    va_start(args, sized_cmd);
    uint8_t param_count = (uint8_t)(sized_cmd >> 8);
    for (uint16_t i = 0; i < (uint16_t)param_count; i++) {
        params[i] = (uint8_t)va_arg(args, int);
    }
    va_end(args);
    lcd_SendSizedCommandRaw(sized_cmd, (const void*)params);
}
void lcd_SendSizedCommandWords(unsigned int sized_cmd, ...) {
    uint16_t params[256];
    va_list args;
    va_start(args, sized_cmd);
    uint8_t param_count = (uint8_t)(sized_cmd >> 8);
    for (uint16_t i = 0; i < (uint16_t)param_count; i++) {
        params[i] = (uint16_t)va_arg(args, int);
    }
    va_end(args);
    lcd_SendSizedCommandRaw(sized_cmd, (const void*)params);
}
void lcd_SetUniformGamma(void) {
    // Gamma of 1.0 is probably incorrect for this function
    PortCE_SPI_State.gamma_curve = 0x8;
}
void lcd_SetDefaultGamma(void) {
    PortCE_SPI_State.gamma_curve = 0x2;
}
