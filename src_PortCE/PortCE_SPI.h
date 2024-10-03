/**
 * @file
 *
 * @author "zerico2005"
 */

#ifndef PORTCE_SPI_H
#define PORTCE_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct SPI_State {
/* Command Table 1 */
	uint8_t gamma_curve : 4; uint8_t : 0; /**< `GC` */
	uint16_t X0_addr : 9; uint8_t : 0;/**< `XS` X Start Address [0 - 320) */
	uint16_t X1_addr : 9; uint8_t : 0;/**< `XE` X End Address [0 - 320) */
	uint16_t Y0_addr : 9; uint8_t : 0;/**< `YS` Y Start Address [0 - 320) */
	uint16_t Y1_addr : 9; uint8_t : 0;/**< `YE` Y End Address [0 - 320) */
	struct {
		uint16_t partial_start_line : 9; uint8_t : 0; /**< `PSL` */
		uint16_t partial_end_line : 9; uint8_t : 0; /**< `PEL` */
	} partial_area;
	struct {
		uint16_t top_fixed_area : 9; uint8_t : 0; /**< `TFA` */
		uint16_t virtical_scrolling_area : 9; uint8_t : 0; /**< `VSA` */
		uint16_t bottom_fixed_area : 9; uint8_t : 0; /**< `BFA` */
	} vertical_scrolling_definition;
	struct {
		uint8_t : 2; /**< Padding */
		uint8_t display_data_latch_order : 1; /**< `MH` */
		uint8_t BGR_order : 1; /**< `RGB` */
		uint8_t line_addr_order : 1; /**< `ML` */
		uint8_t page_column_order : 1; /**< `MV` */
		uint8_t column_addr_order : 1; /**< `MX` */
		uint8_t page_addr_order : 1; /**< `MY` */
	} memory_data_access;
	uint16_t vertical_scroll_start_addr; /**< `VSP` */
	struct {
		uint8_t SPI_format : 3;
		uint8_t RGB_format : 3;
	} interface_pixel_format;
/* Command Table 2 */
	struct {
		uint8_t display_operating_mode : 2; /**< `DM` */
		uint8_t : 2; /**< Padding */
		uint8_t RAM_access_selection : 1; /**< `RM` */
		uint8_t : 0; /**< Padding */
		uint8_t data_transfer_method : 2; /**< `MDT` */
		uint8_t RGB_interface_bus_width : 1; /**< `RIM` */
		uint8_t endianness : 1; /**< `ENDIAN` */
		uint8_t expanded_pixel_format : 2; /**< `EPF` */
		uint8_t SPI_interface_wrap_at_end_mode : 1; /**< `WEMODE0` */
		uint8_t RGB_interface_wrap_at_end_mode : 1; /**< `WEMODE1` */
	} RAM_control;
	struct {
		uint8_t enable_polarity : 1; /**< `EPL` */
		uint8_t dotclk_polarity : 1; /**< `DPL` */
		uint8_t hsync_polarity  : 1; /**< `HSPL` */
		uint8_t vsync_polarity  : 1; /**< `VSPL` */
		uint8_t : 1; /**< `Padding` */
		uint8_t data_enable_mode : 2; /**< `RCM` */
		uint8_t direct_RGB_mode  : 1; /**< `WO` */
		uint8_t vsync_back_porch : 7; uint8_t : 0; /**< `VBP` */
		uint8_t hsync_back_porch : 5; uint8_t : 0; /**< `HBP` */
	} RGB_interface;
	struct {
		uint8_t normal_mode_back_porch_duration  : 6; uint8_t : 0; /**< `BPA` */
		uint8_t normal_mode_front_porch_duration : 6; uint8_t : 0; /**< `FPA` */
		uint8_t separate_porch_control_enable : 1; uint8_t : 0; /**< `PSEN` */
		uint8_t idle_mode_back_porch_duration  : 4; /**< `BPB` */
		uint8_t idle_mode_front_porch_duration : 4; /**< `FPB` */
		uint8_t partial_mode_back_porch_duration  : 4; /**< `BPC` */
		uint8_t partial_mode_front_porch_duration : 4; /**< `FPC` */
	} porch_setting;
	struct {
		uint8_t frame_rate_divisor : 2; /**< `DIV` */
		uint8_t : 2; /**< `Padding` */
		uint8_t separate_frame_rate_control_enable : 1; /**< `FRSEN` */
		uint8_t : 0; /**< Padding */
		uint8_t idle_mode_frame_rate_control : 5; uint8_t : 0; /**< `RTNB` */
		uint8_t idle_mode_polarity_inversion_method : 3; uint8_t : 0; /**< `NLB` */
		uint8_t partial_mode_frame_rate_control : 5; uint8_t : 0; /**< `RTNC` */
		uint8_t partial_mode_polarity_inversion_method : 3; uint8_t : 0; /**< `NLC` */
	} frame_rate_control_1;
	struct {
		uint8_t interval_scan_cycle_length : 4; /**< `ISC` */
		uint8_t interval_scan_enable : 1; /**< `PTGISC` */
		uint8_t : 2; /**< Padding */
		uint8_t non_display_level : 1; /**< `NDL` */
	} partial_control;
	struct {
		uint8_t VGL_setting : 3; /**< `VGLS` */
		uint8_t : 1; /**< Padding */
		uint8_t VGH_setting : 3; /**< `VGHS` */
		uint8_t : 1; /**< Padding */
	} gate_control;
	uint8_t : 1; /**< Padding */
	uint8_t digital_gamma_enable : 1; uint8_t : 0; /**< `DGMEN` */
	uint8_t VCOM_setting : 5; uint8_t : 0; /**< `VCOMS` */
	struct {
		uint8_t invert_gate_control : 1; /**< XGS */
		uint8_t invert_page_column_order : 1; /**< XMV */
		uint8_t invert_display_data_latch_order : 1; /**< XMH */
		uint8_t invert_column_address_order : 1; /**< XMX */
		uint8_t invert_display_inversion : 1; /**< XINV */
		uint8_t invert_BGR_order : 1; /**< XBGR */
		uint8_t invert_page_addr_order : 1; /**< XMY */
	} LCM_control;
} SPI_State;

extern SPI_State PortCE_SPI_State;

void PortCE_reset_SPI_state(const bool TiOS_defaults);

/* Query SPI */

bool PortCE_query_column_major(void);

#ifdef __cplusplus
}
#endif

#endif /* PORTCE_SPI_H */