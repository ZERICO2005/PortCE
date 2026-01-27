#include "PortCE_Common.h"
#include "nano_time.hpp"
#include <usbdrvce.h>
#include <cmath>

usb_error_t usb_Init(
    __attribute__((__unused__)) usb_event_callback_t handler,
    __attribute__((__unused__)) usb_callback_data_t *data,
    __attribute__((__unused__)) const usb_standard_descriptors_t *device_descriptors,
    __attribute__((__unused__)) usb_init_flags_t flags
) {
    return USB_ERROR_NOT_SUPPORTED;
}

void usb_Cleanup(void) {
    return;
}

usb_error_t usb_PollTransfers(void) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_HandleEvents(void) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_WaitForEvents(void) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_WaitForInterrupt(void) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_device_t usb_RefDevice(
    __attribute__((__unused__)) usb_device_t device
) {
    return nullptr;
}

usb_device_t usb_UnrefDevice(
    __attribute__((__unused__)) usb_device_t device
) {
    return nullptr;
}

usb_device_t usb_GetDeviceHub(
    __attribute__((__unused__)) usb_device_t device
) {
    return nullptr;
}

void usb_SetDeviceData(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) usb_device_data_t *data
) {
    return;
}

usb_device_data_t *usb_GetDeviceData(
    __attribute__((__unused__)) usb_device_t device
) {
    return nullptr;
}

usb_device_flags_t usb_GetDeviceFlags(
    __attribute__((__unused__)) usb_device_t device
) {
    return USB_IS_DISABLED;
}

usb_device_t usb_FindDevice(
    __attribute__((__unused__)) usb_device_t root,
    __attribute__((__unused__)) usb_device_t from,
    __attribute__((__unused__)) usb_find_device_flags_t flags
) {
    return nullptr;
}

usb_error_t usb_ResetDevice(
    __attribute__((__unused__)) usb_device_t device
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_DisableDevice(
    __attribute__((__unused__)) usb_device_t device
) {
    return USB_ERROR_NOT_SUPPORTED;
}

uint8_t usb_GetDeviceAddress(
    __attribute__((__unused__)) usb_device_t device
) {
    return 0;
}

int8_t usb_GetDeviceSpeed(
    __attribute__((__unused__)) usb_device_t device
) {
    return USB_SPEED_UNKNOWN;
}

size_t usb_GetConfigurationDescriptorTotalLength(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) uint8_t index
) {
    return 0;
}

usb_error_t usb_GetDescriptor(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) usb_descriptor_type_t type,
    __attribute__((__unused__)) uint8_t index,
    __attribute__((__unused__)) void *descriptor,
    __attribute__((__unused__)) size_t length,
    __attribute__((__unused__)) size_t *transferred
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_SetDescriptor(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) usb_descriptor_type_t type,
    __attribute__((__unused__)) uint8_t index,
    __attribute__((__unused__)) const void *descriptor,
    __attribute__((__unused__)) size_t length
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_GetStringDescriptor(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) uint8_t index,
    __attribute__((__unused__)) uint16_t langid,
    __attribute__((__unused__)) usb_string_descriptor_t *descriptor,
    __attribute__((__unused__)) size_t length,
    __attribute__((__unused__)) size_t *transferred
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_SetStringDescriptor(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) uint8_t index,
    __attribute__((__unused__)) uint16_t langid,
    __attribute__((__unused__)) const usb_string_descriptor_t *descriptor,
    __attribute__((__unused__)) size_t length
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_GetConfiguration(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) uint8_t *index
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_SetConfiguration(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) const usb_configuration_descriptor_t *descriptor,
    __attribute__((__unused__)) size_t length
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_GetInterface(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) uint8_t interface,
    __attribute__((__unused__)) uint8_t *alternate_setting
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_SetInterface(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) const usb_interface_descriptor_t *descriptor,
    __attribute__((__unused__)) size_t length
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_SetEndpointHalt(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_ClearEndpointHalt(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_endpoint_t usb_GetDeviceEndpoint(
    __attribute__((__unused__)) usb_device_t device,
    __attribute__((__unused__)) uint8_t address
) {
    return nullptr;
}

usb_device_t usb_GetEndpointDevice(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return nullptr;
}

void usb_SetEndpointData(
    __attribute__((__unused__)) usb_endpoint_t endpoint,
    __attribute__((__unused__)) usb_endpoint_data_t *data
) {
    return;
}

usb_endpoint_data_t *usb_GetEndpointData(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return nullptr;
}

uint8_t usb_GetEndpointAddress(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return 0xFF;
}

int8_t usb_GetEndpointTransferType(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return 0;
}

size_t usb_GetEndpointMaxPacketSize(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return 0;
}

uint8_t usb_GetEndpointInterval(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return 0;
}

void usb_SetEndpointFlags(
    __attribute__((__unused__)) usb_endpoint_t endpoint,
    __attribute__((__unused__)) usb_endpoint_flags_t flags
) {
    return;
}

usb_endpoint_flags_t usb_GetEndpointFlags(
    __attribute__((__unused__)) usb_endpoint_t endpoint
) {
    return (usb_endpoint_flags_t)0;
}

usb_role_t usb_GetRole(void) {
    return (usb_role_t)0;
}

unsigned usb_GetFrameNumber(void) {
    return 0;
}

usb_error_t usb_ControlTransfer(
    __attribute__((__unused__)) usb_endpoint_t endpoint,
    __attribute__((__unused__)) const usb_control_setup_t *setup,
    __attribute__((__unused__)) void *buffer,
    __attribute__((__unused__)) unsigned retries,
    __attribute__((__unused__)) size_t *transferred
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_Transfer(
    __attribute__((__unused__)) usb_endpoint_t endpoint,
    __attribute__((__unused__)) void *buffer,
    __attribute__((__unused__)) size_t length,
    __attribute__((__unused__)) unsigned retries,
    __attribute__((__unused__)) size_t *transferred
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_ScheduleControlTransfer(
    __attribute__((__unused__)) usb_endpoint_t endpoint,
    __attribute__((__unused__)) const usb_control_setup_t *setup,
    __attribute__((__unused__)) void *buffer,
    __attribute__((__unused__)) usb_transfer_callback_t handler,
    __attribute__((__unused__)) usb_transfer_data_t *data
) {
    return USB_ERROR_NOT_SUPPORTED;
}

usb_error_t usb_ScheduleTransfer(
    __attribute__((__unused__)) usb_endpoint_t endpoint,
    __attribute__((__unused__)) void *buffer,
    __attribute__((__unused__)) size_t length,
    __attribute__((__unused__)) usb_transfer_callback_t handler,
    __attribute__((__unused__)) usb_transfer_data_t *data
) {
    return USB_ERROR_NOT_SUPPORTED;
}

#undef usb_MsToCycles
uint32_t usb_MsToCycles(uint16_t ms) {
    return (uint32_t)ms * UINT32_C(48000);
}

void usb_StopTimer(
    __attribute__((__unused__)) usb_timer_t *timer
) {
    return;
}

void usb_StartTimerCycles(
    __attribute__((__unused__)) usb_timer_t *timer,
    __attribute__((__unused__)) uint32_t timeout_cycles
) {
    return;
}

void usb_RepeatTimerCycles(
    __attribute__((__unused__)) usb_timer_t *timer,
    __attribute__((__unused__)) uint32_t interval_cycles
) {
    return;
}

uint32_t usb_GetCycleCounter(void) {
    double time = getDecimalTime();
    // typically 48MHz
    time *= get_clockspeed();
    // mod 2^32 for casting
    time = fmod(time, 4294967296.0);
    return static_cast<uint32_t>(time);
}

uint24_t usb_GetCounter(void) {
    return static_cast<uint24_t>(usb_GetCycleCounter() >> 8);
}
