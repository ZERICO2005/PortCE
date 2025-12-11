#include <PortCE_Common.h>
#include "PortCE_include/usbdrvce/usbdrvce.h"

#if 0

/**
 * Initializes the usb driver.
 * @param handler Function to be called when a usb event happens.
 * @param data Opaque pointer to be passed to \p handler.
 * @param device_descriptors A pointer to the device descriptors to use, or
 * NULL to use the calculator's defaults.
 * @param flags Which areas of memory to use.
 * @return USB_SUCCESS if initialization succeeded.
 * @note This must be called before any other function, and can be called again
 * to cancel all transfers and disable all devices.
 */
usb_error_t usb_Init(
	usb_event_callback_t handler, usb_callback_data_t *data,
    const usb_standard_descriptors_t *device_descriptors,
    usb_init_flags_t flags
);

/**
 * Uninitializes the usb driver.
 * @note This must be called after calling usb_Init() and before the program
 * exits, even if usb_Init returns an error, or else TIOS gets angry.
 */
void usb_Cleanup(void);

/**
 * This calls any transfer callbacks that have triggered.
 * It is not necessary to call this function, because
 * completed transfers will trigger in any other event
 * dispatch function at the end of the frame.  However,
 * this function may be useful for polling for transfers
 * that may complete early in a frame, without having to wait
 * for the frame to end.  For reference, a frame lasts 1ms.
 * @return An error returned by a callback or USB_SUCCESS.
 */
usb_error_t usb_PollTransfers(void);

/**
 * Calls any device or transfer callbacks that have triggered.
 * @return An error returned by a callback or USB_SUCCESS.
 */
usb_error_t usb_HandleEvents(void);

/**
 * Waits for any device or transfer events to occur, then calls their associated
 * callbacks.
 * @return An error returned by a callback or USB_SUCCESS.
 */
usb_error_t usb_WaitForEvents(void);

/**
 * Waits for any interrupt or usb event to occur, then calls any device or
 * transfer callbacks that may have triggered.
 * @return An error returned by a callback or USB_SUCCESS.
 */
usb_error_t usb_WaitForInterrupt(void);

/**
 * This function may be called to prevent \p device from being automatically
 * freed after its correspending \c USB_DEVICE_DISCONNECTED_EVENT returns.
 * This allows you to continue passing it to other API functions, although
 * many will error with \c USB_ERROR_NO_DEVICE.
 * @param device The device to increase the reference count of.
 * @return device
 */
usb_device_t usb_RefDevice(usb_device_t device);

/**
 * Once this function has been called the same number of times that
 * usb_RefDevice() was called on \p device and the event callback has returned
 * from processing a corresponding \c USB_DEVICE_DISCONNECTED_EVENT, \p device
 * becomes an invalid pointer and may no longer be passed to any API function.
 * @param device The device to decrease the reference count of.
 * @return NULL
 */
usb_device_t usb_UnrefDevice(usb_device_t device);

/**
 * Gets the hub that \p device is attached to, or NULL if \p device is the root
 * hub.
 * @param device Device to get the hub of.
 * @return The hub device or NULL.
 */
usb_device_t usb_GetDeviceHub(usb_device_t device);

/**
 * Sets the user data associated with \p device.
 * @param device Device to set the user data of.
 * @param data Data to set.
 */
void usb_SetDeviceData(usb_device_t device, usb_device_data_t *data);

/**
 * Gets the user data associated with \p device.
 * @param device Device to get the user data of.
 * @return The user data last set with \c usb_SetDeviceData.
 */
usb_device_data_t *usb_GetDeviceData(usb_device_t device);

/**
 * This returns the same flags that are used by usb_FindDevice() for a given
 * \p device.  Note that \c USB_SKIP_ATTACHED is not part of these flags.
 * @param device The device to get the flags of.
 * @return The \c usb_device_flags_t flags associated with \p device.
 */
usb_device_flags_t usb_GetDeviceFlags(usb_device_t device);

/**
 * Finds the next device connected through \p root after \p from satisfying
 * \p flags, or \c NULL if no more matching devices.
 *
 * To enumerate all devices, excluding all hubs:
 * \code
 * usb_device_t device = NULL;
 * while ((device = usb_FindDevice(NULL, device, USB_SKIP_HUBS))) {
 *   handle(device);
 * }
 * \endcode
 *
 * To enumerate all hubs and devices, including the root hub:
 * \code
 * usb_device_t device = NULL;
 * while ((device = usb_FindDevice(NULL, device, USB_SKIP_NONE))) {
 *   handle(device);
 * }
 * \endcode
 *
 * To enumerate all hubs and devices except the root hub:
 * \code
 * usb_device_t device = NULL; // same as using usb_RootHub()
 * while ((device = usb_FindDevice(usb_RootHub(), device, USB_SKIP_NONE))) {
 *   handle(device);
 * }
 * \endcode
 *
 * To enumerate all devices below a specific hub:
 * \code
 * usb_device_t device = NULL; // same as using hub
 * while ((device = usb_FindDevice(hub, device, USB_SKIP_NONE))) {
 *   handle(device);
 * }
 * \endcode
 *
 * To enumerate all disabled hubs directly attached to a specific hub:
 * \code
 * usb_device_t device = NULL; // must not use hub or else USB_SKIP_ATTACHED
 *                             // will skip all devices attached to hub!
 * while ((device = usb_FindDevice(hub, device, USB_SKIP_ENABLED |
 *                                 USB_SKIP_DEVICES | USB_SKIP_ATTACHED))) {
 *   handle(device);
 * }
 * \endcode
 * @param root Hub below which to limit search, or \c NULL to search all
 * devices including the root hub.
 * @param from The device to start the search from, or \c NULL to start from
 * \p root and include devices attached to root even with \c USB_SKIP_ATTACHED.
 * @param flags What kinds of devices to return.
 * @return The next device connected through \p root after \p from satisfying
 * \p flags or \c NULL if none.
 */
usb_device_t usb_FindDevice(
	usb_device_t root, usb_device_t from,
    usb_find_device_flags_t flags
);

/**
 * Performs an asynchronous usb reset on a device. This triggers a device
 * enabled event when the reset is complete.
 * @param device The device to reset.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_ResetDevice(usb_device_t device);

/**
 * Forces a device to become disabled. This triggers a device disabled event
 * when it finishes.
 * @param device The device to disable.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_DisableDevice(usb_device_t device);

/**
 * Gets the usb address of a \p device, or 0 if disabled.
 * @param device The device to communicate with.
 * @return The usb address or 0.
 */
uint8_t usb_GetDeviceAddress(usb_device_t device);

/**
 * Gets the speed of a device, or USB_SPEED_UNKNOWN if unknown.
 * @param device The device to communicate with.
 * @return The \c usb_speed_t.
 */
int8_t usb_GetDeviceSpeed(usb_device_t device);

/**
 * Determines how large of a buffer would be required to receive the complete
 * configuration descriptor at \p index.
 * @note Blocks while the configuration descriptor is fetched.
 * @param device The device to communicate with.
 * @param index Which configuration descriptor to query.
 * @return The total length in bytes of the combined configuration descriptor or
 * 0 on error.
 */
size_t usb_GetConfigurationDescriptorTotalLength(
	usb_device_t device, uint8_t index
);

/**
 * Gets the descriptor of a \p device of \p type at \p index.
 * @note Blocks while the descriptor is fetched.
 * @param device The device to communicate with.
 * @param type The \c usb_descriptor_type_t to fetch.
 * @param index Descriptor index to fetch.
 * @param descriptor Returns the fetched descriptor.
 * @param length The maximum number of bytes to receive.
 * The \p descriptor buffer must be at least this large.
 * @param transferred NULL or returns the number of bytes actually received.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_GetDescriptor(
	usb_device_t device, usb_descriptor_type_t type,
    uint8_t index, void *descriptor, size_t length,
    size_t *transferred
);

/**
 * Changes the descriptor at \p index.
 * @note Blocks while the descriptor is changed.
 * @note Devices do not usually support this.
 * @param device The device to communicate with.
 * @param type The \c usb_descriptor_type_t to change.
 * @param index The descriptor index to change.
 * @param descriptor The new descriptor.
 * @param length The number of bytes in the new descriptor.
 * The \p descriptor buffer must be this large.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_SetDescriptor(
	usb_device_t device, usb_descriptor_type_t type,
    uint8_t index, const void *descriptor,
    size_t length
);

/**
 * Gets the string descriptor at \p index and \p langid.
 * @note Blocks while the descriptor is fetched.
 * @param device The device to communicate with.
 * @param index Descriptor index to fetch.
 * @param langid Language ID to fetch.
 * @param descriptor Returns the fetched descriptor.
 * @param length The number of bytes to transfer.
 * The \p descriptor buffer must be at least this large.
 * @param transferred NULL or returns the number of bytes actually received.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_GetStringDescriptor(
	usb_device_t device, uint8_t index,
    uint16_t langid,
    usb_string_descriptor_t *descriptor,
    size_t length, size_t *transferred
);

/**
 * Sets the string descriptor at \p index and \p langid.
 * @note Blocks while the descriptor is changed.
 * @note Devices do not usually support this.
 * @param device The device to communicate with.
 * @param index Descriptor index to change.
 * @param langid Language ID to change.
 * @param descriptor The new descriptor.
 * @param length The number of bytes to transfer.
 * The \p descriptor buffer must be this large.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_SetStringDescriptor(
	usb_device_t device, uint8_t index,
    uint16_t langid,
	const usb_string_descriptor_t *descriptor,
    size_t length
);

/**
 * Gets the currently active configuration of a device.
 * @param device The device to communicate with.
 * @param index Returns the current configuration value, or 0 if unconfigured.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_GetConfiguration(usb_device_t device, uint8_t *index);

/**
 * Selects the configuration specified by the \p configuration_descriptor.
 * This must be called before pipes other than the default control pipe can be
 * accessed.  Calling this function invalidates all \c usb_endpoint_t pointers
 * corresponding with \p device except for any referring to its default control
 * pipe.
 * @param device The device to communicate with.
 * @param descriptor A complete combined configuration descriptor fetched with
 * usb_GetDescriptor().
 * @param length The total length of the configuration descriptor.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_SetConfiguration(
	usb_device_t device,
    const usb_configuration_descriptor_t *descriptor,
    size_t length
);

/**
 * Gets the current alternate setting in use on the specified interface.
 * @param device The device to communicate with.
 * @param interface Interface index to query.
 * @param alternate_setting Returns the alternate setting in use.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_GetInterface(
	usb_device_t device, uint8_t interface,
    uint8_t *alternate_setting
);

/**
 * Sets the alternate setting to use for its corresponding interface.  Calling
 * this function invalidates any \p usb_endpoint_t pointers corresponding with
 * the endpoints that were part of the previously selected alternate setting.
 * @param device The device to communicate with.
 * @param descriptor The interface descriptor describing the alternate
 * setting to select within a configuration descriptor.
 * @param length The remaining length of the configuration descriptor after
 * the beginning of the \p interface_descriptor.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_SetInterface(
	usb_device_t device,
    const usb_interface_descriptor_t *descriptor,
    size_t length
);

/**
 * Sets halt condition on \p endpoint.  This is only supported by bulk and
 * interrupt endpoints.  If acting as usb host, this may only be called if there
 * are no pending transfers.  This also has the sife effect of asynchronously
 * cancelling all pending transfers to \p endpoint.
 * @param endpoint The endpoint to set the halt condition of.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_SetEndpointHalt(usb_endpoint_t endpoint);

/**
 * Clears halt condition on \p endpoint.  This is only supported by bulk and
 * interrupt endpoints.  If acting as usb host, this may only be called if there
 * are no pending transfers.  If any non-control transfer stalls, this is called
 * automatically, so you only need to call this if you need to clear an endpoint
 * halt for a reason other than a stalled transfer. This function blocks until
 * the halt condition is cleared.
 * @param endpoint The endpoint to clear the halt condition of.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_ClearEndpointHalt(usb_endpoint_t endpoint);

/**
 * Gets the endpoint of a \p device with a given \p address, or NULL if that
 * address is unused.
 * @param device Device to get the user data of.
 * @param address Address of the endpoint to get.
 * @return The specified endpoint or NULL.
 */
usb_endpoint_t usb_GetDeviceEndpoint(usb_device_t device, uint8_t address);

/**
 * Gets the device that \p endpoint is connected to.
 * @param endpoint Endpoint to get the device of.
 * @return The device for an \p endpoint.
 */
usb_device_t usb_GetEndpointDevice(usb_endpoint_t endpoint);

/**
 * Sets the user data associated with \p endpoint.
 * @param endpoint Endpoint to set the user data of.
 * @param data Data to set.
 */
void usb_SetEndpointData(usb_endpoint_t endpoint, usb_endpoint_data_t *data);

/**
 * Gets the user data associated with \p endpoint.
 * @param endpoint Endpoint to get the user data of.
 * @return The user data last set with \c usb_SetEndpointData.
 */
usb_endpoint_data_t *usb_GetEndpointData(usb_endpoint_t endpoint);

/**
 * Gets the address of an endpoint.
 * @param endpoint The endpoint to get the address of.
 * @return The address of an \p endpoint or 0xFF on error.
 */
uint8_t usb_GetEndpointAddress(usb_endpoint_t endpoint);

/**
 * Gets the transfer type of an endpoint.
 * @param endpoint The endpoint to get the transfer type of.
 * @return The \c usb_transfer_type of an endpoint.
 */
int8_t usb_GetEndpointTransferType(usb_endpoint_t endpoint);

/**
 * Gets the maximum packet size of an endpoint.
 * @param endpoint The endpoint to get the maximum packet size of.
 * @return The \c wMaxPacketSize of an \p endpoint.
 */
size_t usb_GetEndpointMaxPacketSize(usb_endpoint_t endpoint);

/**
 * Gets the interval of an endpoint.
 * @param endpoint The endpoint to get the endpoint of.
 * @return The actual \c bInterval of an \p endpoint, which is rounded down to
 * the nearest power of two from the descriptor, or 0 for asynchronous
 * endpoints.
 */
uint8_t usb_GetEndpointInterval(usb_endpoint_t endpoint);

/**
 * Sets the flags for an endpoint.
 * @param endpoint The endpoint to set the flags of.
 * @param flags The \c usb_endpoint_flags_t to set.
 */
void usb_SetEndpointFlags(usb_endpoint_t endpoint, usb_endpoint_flags_t flags);

/**
 * Gets the flags for an endpoint.
 * @param endpoint The endpoint to get the flags of.
 * @return The \c usb_endpoint_flags_t last set with \c usb_SetEndpointFlags.
 */
usb_endpoint_flags_t usb_GetEndpointFlags(usb_endpoint_t endpoint);

/**
 * Returns the current role the usb hardware is operating in.
 * @return The \c usb_role_t of the current role.
 */
usb_role_t usb_GetRole(void);

/**
 * Returns the current 11-bit frame number, as last broadcast by the current
 * host, multiplied by 8.  This value ranges from 0x0000 to 0x3FF8, increases by
 * 8 every 1 ms, is truncated to 14 bits, and is synchronized with the host usb
 * clock.
 * @warning The bottom 3 bits are usually 0, but this is not guaranteed because
 * random mmio writes could affect those bits.
 * @note If the hardware supported full speed usb, the lower 3 bits would be the
 * microframe number.
 * @return usb_frame_number << 3
 */
unsigned usb_GetFrameNumber(void);

/**
 * Schedules a transfer to the pipe connected to \p endpoint, in the direction
 * indicated by \p setup->bmRequestType, using \p buffer as the data buffer,
 * \p setup->wLength as the buffer length, and then waits for it to complete.
 * If acting as usb host and using a control pipe, \p setup is used as the setup
 * packet, otherwise all fields not mentioned above are ignored.
 * @param endpoint The endpoint to communicate with, which also specifies the
 * direction for non-control transfers.
 * @param setup Indicates the transfer direction and buffer length.  If acting
 * as usb host and using a control pipe, also used as the setup packet to send.
 * @param buffer Data to transfer that must reside in RAM and have room for at
 * least \p setup->wLength bytes.
 * @param retries How many times to retry the transfer before timing out.
 * If retries is USB_RETRY_FOREVER, the transfer never times out.
 * @param transferred NULL or returns the number of bytes actually received.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_ControlTransfer(
	usb_endpoint_t endpoint, const usb_control_setup_t *setup,
    void *buffer, unsigned retries, size_t *transferred
);

/**
 * Schedules a transfer to the pipe connected to \p endpoint, using \p length as
 * the buffer length, and waits for it to complete. If acting as usb host and
 * using a control pipe, uses the beginning of \p buffer as the setup packet to
 * send, its \c bmRequestType for the transfer direction, and the rest of
 * \p buffer as the data buffer.  Otherwise, uses \p endpoint for transfer
 * direction and  the whole \p buffer as the data buffer.
 * @param endpoint The endpoint to communicate with, which also specifies the
 * direction for non-control transfers.
 * @param buffer Data to transfer that must reside in RAM.  When acting as usb
 * host and using a control pipe, starts with a \c usb_control_setup_t.
 * @param length Number of bytes to transfer.  The \p buffer must be at least
 * this large.  However, this is ignored for control transfers when acting as
 * usb host where the \c wLength of the setup packet is used instead.
 * @param retries How many times to retry the transfer before timing out.
 * If retries is USB_RETRY_FOREVER, the transfer never times out.
 * @param transferred NULL or returns the number of bytes actually received.
 * NULL means don't return anything.
 * @return USB_SUCCESS if the transfer succeeded or an error.
 */
usb_error_t usb_Transfer(
	usb_endpoint_t endpoint, void *buffer, size_t length,
    unsigned retries, size_t *transferred
);

/**
 * Schedules a transfer to the pipe connected to \p endpoint, in the direction
 * indicated by \p setup->bmRequestType, using \p buffer as the data buffer, and
 * \p setup->wLength as the buffer length.  If acting as usb host and using a
 * control pipe, \p setup is used as the setup packet, otherwise all fields not
 * mentioned above are ignored.
 * @param endpoint The endpoint to communicate with, which also specifies the
 * direction for non-control transfers.
 * @param setup Indicates the transfer direction and buffer length.  If acting
 * as usb host and using a control pipe, also used as the setup packet to send.
 * @param buffer Data to transfer that must reside in RAM.  This buffer must
 * remain valid until the callback is called i.e. it cannot be modified or
 * freed.
 * @param handler Function to be called when the transfer finishes.
 * @param data Opaque pointer to be passed to the \p handler.
 * @return USB_SUCCESS if the transfer was scheduled or an error.
 */
usb_error_t usb_ScheduleControlTransfer(
	usb_endpoint_t endpoint,
    const usb_control_setup_t *setup, void *buffer,
    usb_transfer_callback_t handler,
    usb_transfer_data_t *data
);

/**
 * If endpoint is not a control endpoint, schedules a transfer of the endpoint's
 * transfer type.  If the endpoint is a control endpoint, schedules a control
 * transfer interpreting the beginning of buffer as the \c usb_control_setup_t
 * and using the rest of the buffer as the transfer buffer.
 * @param endpoint The endpoint to communicate with, which also specifies the
 * direction for non-control transfers.
 * @param buffer Data to transfer that must reside in RAM.  When acting as usb
 * host and using a control pipe, starts with a \c usb_control_setup_t.  This
 * buffer must remain valid until the callback is called i.e. it cannot be
 * modified or freed.
 * @param length Number of bytes to transfer.  The \p buffer must be at least
 * this large.  However, this is ignored for control transfers when acting as
 * usb host where the \c wLength of the setup packet is used instead.
 * @param handler Function to be called when the transfer finishes.
 * @param data Opaque pointer to be passed to the \p handler.
 * @return USB_SUCCESS if the transfer was scheduled or an error.
 */
usb_error_t usb_ScheduleTransfer(
	usb_endpoint_t endpoint, void *buffer, size_t length,
    usb_transfer_callback_t handler, usb_transfer_data_t *data
);

/* Timer Functions */

/**
 * Stops a timer.
 * @note May be called from within \c timer->handler itself.
 */
void usb_StopTimer(usb_timer_t *timer);

/**
 * Starts a timer that expires \p timeout_cycles after calling this function.
 * @note May be called from within \c timer->handler itself.
 * @param timer A user allocated struct with \c timer->handler already initialized.
 * @param timeout_cycles Timeout in cpu cycles.
 */
void usb_StartTimerCycles(usb_timer_t *timer, uint32_t timeout_cycles);

/**
 * Starts a timer that expires \p interval_cycles after it last expired.
 * @note May be called from within \c timer->handler itself.
 * @param timer A user allocated struct with \c timer->handler already initialized.
 * @param interval_cycles Repeat interval in cpu cycles.
 */
void usb_RepeatTimerCycles(usb_timer_t *timer, uint32_t interval_cycles);

#endif

/**
 * Returns a counter that increments once every cpu cycle, or 48000 times every
 * millisecond.  This counter overflows every 90 seconds or so.
 * @return Cpu cycle counter.
 */
uint32_t usb_GetCycleCounter(void) {
	double time = getDecimalTime();
	// time *= Ti84CE_Clockspeed;
	time *= 48.0e6; // 48Mhz
	// mod 2^32 for casting
	time = fmod(time, 4294967296.0);
	return (uint32_t)time;
}

/**
 * Returns a counter that increments once every 256 cpu cycles, or 375 times
 * every 2 milliseconds.  This is the high 24 bits of the same counter returned
 * by usb_GetCycleCounter().
 * @return Cpu cycle counter >> 8.
 */
uint24_t usb_GetCounter(void) {
	return (uint24_t)(usb_GetCycleCounter() >> 8);
}
